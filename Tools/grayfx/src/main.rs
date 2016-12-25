extern crate xml;
extern crate clap;

mod polyfill;
mod emitter;

use emitter::Emitter;

use std::fs::File;
use std::io::BufReader;
use std::collections::HashMap;

use xml::reader::{EventReader, XmlEvent};
use clap::{Arg, App};

fn matching_element_id<'a>(ids: &str, attr: &'a Vec<xml::attribute::OwnedAttribute>) -> Option<&'a str> {
    let id_attr = match attr.iter().find(|ref a| a.name.local_name == "id") {
        Some(a) => a,
        None => return None
    };

    if ids.is_empty() || ids.split(',').any(|i| id_attr.value == i) {
        Some(&id_attr.value)
    }
    else { None }
}

fn main() {
    let app: App = App::new("grayfx")
        .version("0.0")
        .author("Nigel Baillie <metreckk@gmail.com>")
        .about("Converts Inkscape SVGs into C++ rendering and/or physics code for Gray")
        .arg(Arg::with_name("input")
            .help("the svg file to parse")
            .short("f")
            .takes_value(true)
            .index(1)
            .required(true))
        .arg(Arg::with_name("ids")
            .short("i")
            .help("comma-separated ids of the elements to process")
            .takes_value(true)
            .required(false))
        .arg(Arg::with_name("color")
            .short("c")
            .help("expression to use for color")
            .default_value("Color4f::WHITE"))
         .arg(Arg::with_name("var")
             .short("v")
             .help("variable name to use in generated code")
             .default_value("draw"));
    let matches = app.get_matches();

    let filename = matches.value_of("input").unwrap();
    let file = File::open(filename).unwrap();
    let file = BufReader::new(file);

    let ids = matches.value_of("ids").unwrap_or("");
    let color = matches.value_of("color").unwrap();

    let ids_count = if ids.is_empty() {
        0
    } else {
        ids.chars().fold(1, |n, c| if c == ',' { n + 1 } else { n })
    };

    // === Parse XML ===
    let mut matching_elements = HashMap::<String, (String, Vec<xml::attribute::OwnedAttribute>)>::new();
    let parser = EventReader::new(file);
    'parse_xml: for xmlevent in parser {
        match xmlevent {
            // <path> elements:
            Ok(XmlEvent::StartElement { name, attributes, .. }) => {
                if let Some(id) = matching_element_id(ids, &attributes) {
                    matching_elements.insert(id.to_string(), (name.local_name, attributes.clone()));

                    // Duck out early if possible
                    if ids_count > 0 && matching_elements.len() >= ids_count {
                        break 'parse_xml;
                    }
                }
            }
            /*
            Ok(XmlEvent::EndElement { name }) => {
                println!("END {}", name);
            }
            */
            Err(e) => {
                println!("// Error: {}", e);
                break;
            }
            _ => {}
        }
    }

    // === Now process elements that matched, in specified order ===
    let draw_var = matches.value_of("var").unwrap();
    let mut state = Emitter::new();
    let mut process_element = |id: String, el: &(String, Vec<xml::attribute::OwnedAttribute>)| {
        match el {
            &(ref name, ref attributes) => {
                if name == "path" {
                    let d_attr = attributes.iter().find(|ref a| a.name.local_name == "d")
                        .expect(&format!("Path with id={} doesn't have a `d` attribute", id));

                    state.emit_path(&d_attr.value, &id, &draw_var, color);
                }
                else if name == "circle" {
                    //                 cx    cy    r
                    let mut params = (None, None, None);

                    for ref attr in attributes {
                        let name: &str = &attr.name.local_name;
                        match name {
                            "cx" => params.0 = Some(attr.value.clone()),
                            "cy" => params.1 = Some(attr.value.clone()),
                            "r"  => params.2 = Some(attr.value.clone()),
                            _ => {}
                        }
                    }

                    if let (Some(cx), Some(cy), Some(r)) = params {
                        state.emit_circle(&cx, &cy, &r, &id, &draw_var, color);
                    }
                    else { panic!("Invalid circle (lacking 'cx', 'cy', or 'r')") }
                }
            }
        }
    };

    if ids_count > 0 {
        let each_id = ids.split(',');
        for id in each_id {

            let id_str = id.to_string();
            if !matching_elements.contains_key(&id_str) {
                panic!("ID not found in {}: {}", id_str, filename);
            }

            process_element(id_str, &matching_elements[id])
        }
    }
    else {
        for (ref id, ref element) in matching_elements {
            process_element(id.clone(), &element)
        }
    }
}
