extern crate xml;
extern crate clap;

mod polyfill;
mod emitter;

use emitter::Emitter;

use std::fs::File;
use std::io::BufReader;

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
        .version("0.1.0")
        .author("Nigel Baillie <metreckk@gmail.com>")
        .about("Converts Inkscape SVGs into C++ rendering and/or physics code for Gray")
        .arg(Arg::with_name("input")
            .help("the svg file to parse")
            .short("f")
            .index(1)
            .takes_value(true)
            .required(true))
        .arg(Arg::with_name("ids")
            .short("i")
            .help("comma-separated ids of the elements to process")
            .takes_value(true)
            .required(false))
        .arg(Arg::with_name("color")
            .short("c")
            .help("expression to use for color")
            .takes_value(true)
            .required(false))
         .arg(Arg::with_name("drawvar")
             .short("d")
             .help("generate rendering code with this variable name")
             .takes_value(true)
             .required(false))
         .arg(Arg::with_name("physicsvar")
             .short("p")
             .help("generate physics setup code with this variable name")
             .takes_value(true)
             .required(false));
    let matches = app.get_matches();

    let filename = matches.value_of("input").unwrap();
    let file = File::open(filename).unwrap();
    let file = BufReader::new(file);

    let ids = matches.value_of("ids").unwrap_or("");
    let color = matches.value_of("color");

    let ids_count = if ids.is_empty() {
        0
    } else {
        ids.chars().fold(1, |n, c| if c == ',' { n + 1 } else { n })
    };

    // === Parse XML ===
    let mut emitter = Emitter::new();
    let parser = EventReader::new(file);
    'parse_xml: for xmlevent in parser {
        match xmlevent {
            // elements:
            Ok(XmlEvent::StartElement { name, attributes, .. }) => {
                if let Some(id) = matching_element_id(ids, &attributes) {
                    emitter.add_shape(id, &name.local_name, &attributes);

                    // Duck out early if possible
                    if ids_count > 0 && emitter.len() >= ids_count {
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

    // TODO TODO TODO
    //   Next step is to parse color from style and tag collision shapes appropriately
    // TODO TODO TODO

    // === Now process elements that matched, in specified order ===
    let draw_var = matches.value_of("drawvar");
    let phys_var = matches.value_of("physicsvar");

    if ids_count > 0 {
        let each_id = ids.split(',');
        for id in each_id {

            let id_str = id.to_owned();
            if !emitter.emit(&id_str, draw_var, phys_var, color) {
                panic!("ID \"{}\" not found in {}", id_str, filename);
            }
        }
    }
    else {
        emitter.emit_all(draw_var, phys_var, color);
    }
}
