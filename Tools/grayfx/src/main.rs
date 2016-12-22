extern crate xml;
extern crate clap;

mod polyfill;

use std::fs::File;
use std::io::BufReader;
use std::str::FromStr;
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

fn emit_path(d: &str, id: &str, drawnode: &str, color: &str) {
    // Fill a vec with all verts
    let mut n = 0;
    let mut verts = Vec::with_capacity(d.len() / 5);
    {
        let mut exprs = d.split(' ');
        let mut current_pos = [0.0, 0.0];
        verts.push(current_pos);
        while let Some(expr) = exprs.next() {
            if expr == "m" {
                if n == 0 {
                    // Skip the starting point
                    exprs.next();
                    continue;
                }
                else { panic!("'m' showing up more than once???") }
            }
            else if expr == "z" {
                break
            }
            else if !expr.contains(',') {
                panic!("Unsupported expression: {}", expr);
            }

            let mut xy = expr.split(',');
            let x = xy.next().expect("Invalid pair");
            let y = xy.next().expect("Invalid pair");
            current_pos = [
                current_pos[0] + f64::from_str(x).expect("Failed to parse float"),
                current_pos[1] - f64::from_str(y).expect("Failed to parse float")
            ];
            verts.push(current_pos);

            n = n + 1;
        }
    }

    // Triangulation and c++ code emission
    println!("// Triangles for {}", id);
    let triangles = polyfill::triangle_indices(&verts, polyfill::TANGENTIAL);
    for t in triangles {
        println!(
            "{}->drawTriangle(Vec2({},{}), Vec2({},{}), Vec2({},{}), {});",
            drawnode,
            verts[t[0]][0],verts[t[0]][1],
            verts[t[1]][0],verts[t[1]][1],
            verts[t[2]][0],verts[t[2]][1],
            color
        );
    }
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
            .default_value("Color4f::WHITE"));
    let matches = app.get_matches();

    let file = File::open(matches.value_of("input").unwrap()).unwrap();
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
    let process_element = |id: String, el: &(String, Vec<xml::attribute::OwnedAttribute>)| {
        match el {
            &(ref name, ref attributes) => {
                if name == "path" {
                    let d_attr = attributes.iter().find(|ref a| a.name.local_name == "d")
                        .expect(&format!("Path with id={} doesn't have a `d` attribute", id));

                    emit_path(&d_attr.value, &id, "draw", color);
                }
                else if name == "circle" {
                    panic!("Unimplemented TODO!!!");
                }
            }
        }
    };

    if ids_count > 0 {
        let each_id = ids.split(',');
        for id in each_id { process_element(id.to_string(), &matching_elements[id]) }
    }
    else {
        for (ref id, ref element) in matching_elements {
            process_element(id.clone(), &element)
        }
    }
}
