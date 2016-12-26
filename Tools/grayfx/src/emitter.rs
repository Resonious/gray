extern crate xml;

use polyfill;

use std::str::FromStr;
use std::collections::HashMap;

/// Parse a float with appropriate panic message on failure.
macro_rules! parse_float {
    ($s:expr) => (f64::from_str($s).expect("Failed to parse float"))
}


trait Shape {
    fn emit(&self, id: &str, drawnode: &str, color: &str);
}

pub struct Polygon {
    verts:     Vec<[f64; 2]>,
    triangles: Vec<[usize; 3]>
}

impl Shape for Polygon {
    fn emit(&self, id: &str, drawnode: &str, color: &str) {
        println!("// Triangles for {}", id);
        for ref t in &self.triangles {
            println!(
                "{}->drawTriangle(Vec2({:.10}f,{:.10}f), Vec2({:.10}f,{:.10}f), Vec2({:.10}f,{:.10}f), {});",
                Emitter::varname(id, drawnode),
                self.verts[t[0]][0], self.verts[t[0]][1],
                self.verts[t[1]][0], self.verts[t[1]][1],
                self.verts[t[2]][0], self.verts[t[2]][1],
                color
            );
        }
    }
}

pub struct Circle {
    cx: f64,
    cy: f64,
    r:  f64
}

impl Shape for Circle {
    fn emit(&self, id: &str, drawnode: &str, color: &str) {
        println!("// Circle for {}", id);
        // arguments: center, radius, angle, segments, color
        println!(
            "{}->drawSolidCircle(Vec2({:.10}f,{:.10}f), {:.10}f, 0.0f, 20, {});",
            Emitter::varname(id, drawnode),
            self.cx, self.cy, self.r,
            color
        );
    }
}

/// The emitter itself. Holds onto shapes relative to their id.
pub struct Emitter {
    origin: Option<[f64; 2]>,
    shapes: HashMap<String, Box<Shape>>
}

impl Emitter {
    pub fn new() -> Emitter {
        Emitter {
            origin: None,
            shapes: HashMap::new()
        }
    }

    ///
    /// Emit a single shape to stdout.
    /// Returns true if a shape under `id` was found and emitted.
    /// Returns false if there was no shape under `id`.
    ///
    pub fn emit(&self, id: &String, drawnode: &str, color: &str) -> bool {
        match self.shapes.get(id) {
            Some(shape) =>{
                shape.emit(id, drawnode, color);
                true
            }
            None => false
        }
    }

    ///
    /// Emit all shapes to stdout.
    ///
    pub fn emit_all(&self, drawnode: &str, color: &str) {
        for (ref id, ref shape) in &self.shapes {
            shape.emit(id, drawnode, color);
        }
    }

    ///
    /// How many shapes we've added.
    ///
    pub fn len(&self) -> usize {
        self.shapes.len()
    }

    ///
    /// Add a shape from xml attributes.
    ///
    pub fn add_shape(&mut self, id: &str, tag: &str, attributes: &Vec<xml::attribute::OwnedAttribute>) {
        let new_shape: Box<Shape> = match tag {
            "path"   => Box::new(self.parse_polygon(attributes)),
            "circle" => Box::new(self.parse_circle(attributes)),
            _        => return
        };
        self.shapes.insert(id.to_owned(), new_shape);
    }

    ///
    /// Parse a polygon with vertex positions based on `self.origin`.
    ///
    pub fn parse_polygon(&mut self, attributes: &Vec<xml::attribute::OwnedAttribute>) -> Polygon {
        let ref d = attributes.iter().find(|ref a| a.name.local_name == "d")
            .expect("Path lacking 'd' attribute")
            .value;

        // Fill a vec with all verts
        let mut current_pos = [0.0, 0.0];
        let mut verts = Vec::with_capacity(d.len() / 5);
        {
            let mut n = 0;
            let mut exprs = d.split(' ');

            while let Some(expr) = exprs.next() {
                if expr == "m" {
                    if n == 0 {
                        // Starting point is origin
                        let expr = exprs.next().unwrap();
                        let mut xy = expr.split(',');
                        let sx = xy.next().expect("Invalid pair");
                        let sy = xy.next().expect("Invalid pair");
                        self.assign_position_from_origin(&mut current_pos, sx, sy);
                        verts.push(current_pos);
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
                current_pos[0] += parse_float!(x);
                current_pos[1] -= parse_float!(y);
                verts.push(current_pos);

                n = n + 1;
            }
        }

        Polygon {
            triangles: polyfill::triangle_indices(&verts, polyfill::TANGENTIAL),
            verts: verts
        }
    }

    ///
    /// Parse a circle with center point based on `self.origin`.
    ///
    pub fn parse_circle(&mut self, attributes: &Vec<xml::attribute::OwnedAttribute>) -> Circle {
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

            if let (Some(cx), Some(cy), Some(r)) = params {
                let mut cxy = [0.0, 0.0];
                self.assign_position_from_origin(&mut cxy, &cx, &cy);
                return Circle { cx: cxy[0], cy: cxy[1], r: parse_float!(&r) };
            }
        }
        panic!("Invalid circle");
    }

    // =========== Private helper functions ===========

    fn varname(id: &str, drawnode: &str) -> String {
        if drawnode.contains("{id}") {
            drawnode.to_owned().replace("{id}", id)
        }
        else {
            drawnode.to_owned()
        }
    }

    fn assign_position_from_origin(&mut self, current_pos: &mut [f64; 2], sx: &str, sy: &str) {
        let x =  parse_float!(sx);
        let y = -parse_float!(sy);

        // Set this point to origin if we have none already,
        // otherwise offset by this starting point.
        match self.origin {
            Some(origin) => *current_pos = [x - origin[0], y - origin[1]],
            None         => self.origin = Some([x, y])
        }
    }
}
