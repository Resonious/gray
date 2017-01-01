extern crate xml;

use polyfill;

use std::str::FromStr;
use std::collections::BTreeMap;

/// Parse a float with appropriate panic message on failure.
macro_rules! parse_float {
    ($s:expr) => (f64::from_str($s).expect("Failed to parse float"))
}

pub struct Color {
    r: u8, g: u8, b: u8
}

impl Color {
    pub fn black() -> Color { Color { r: 0, g: 0, b: 0 } }
    // pub fn white() -> Color { Color { r: u8::max_value(), g: u8::max_value(), b: u8::max_value() } }

    pub fn parse(hexcode: &str) -> Color {
        let mut chars = hexcode.chars();

        // Skip over the leading '#'
        {
            let hash = chars.next().expect("Empty string passed as color hexcode");
            assert_eq!(hash, '#');
        }

        // Grab 2 chars, parse those as hexadecimal
        let mut next_component = || {
            let a = chars.next().expect("Ran out of chars while parsing color hexcode");
            let b = chars.next().expect("Ran out of chars while parsing color hexcode");
            let ab = String::from_utf8(vec![a as u8, b as u8]).expect("What");
            u8::from_str_radix(&ab, 16).expect("Invalid hex number in color string")
        };

        let r = next_component();
        let g = next_component();
        let b = next_component();

        Color { r:r, g:g, b:b }
    }

    pub fn emit(&self) -> String {
        format!(
            "cocos2d::Color4F(cocos2d::Color3B({},{},{}))",
            self.r, self.g, self.b
        )
    }
}


trait Shape {
    /// Should generate code to draw the shape on a cocos2dx DrawNode
    fn emit_graphics(&self, id: &str, drawnode: &str, color: Option<&str>);
    /// Should generate code for edge segments, and an encompassing shape
    ///
    /// TODO probably also make those generated shapes have certain categories
    /// and the encompassing should be a sensor.
    fn emit_physics(&self, id: &str, physicsbody: &str);
}


pub struct Polygon {
    verts:     Vec<[f64; 2]>,
    triangles: Vec<[usize; 3]>,
    color:     Color
}

impl Shape for Polygon {
    fn emit_graphics(&self, id: &str, drawnode: &str, color: Option<&str>) {
        println!("// Triangles for {}", id);
        for ref t in &self.triangles {
            println!(
                "{}->drawTriangle(Vec2({:.10}f,{:.10}f), Vec2({:.10}f,{:.10}f), Vec2({:.10}f,{:.10}f), {});",
                Emitter::varname(id, drawnode),
                self.verts[t[0]][0], self.verts[t[0]][1],
                self.verts[t[1]][0], self.verts[t[1]][1],
                self.verts[t[2]][0], self.verts[t[2]][1],
                color.unwrap_or(&self.color.emit())
            );
        }
    }

    fn emit_physics(&self, id: &str, physicsbody: &str) {
        println!("// Physics for {}", id);
        if self.verts.len() <= 1 {
            println!("// {} does not have enough vertices for a polygon", id);
            return;
        }

        let mut verts   = self.verts.iter();
        let mut vert_a  = verts.next();
        let mut vert_b  = verts.next();
        let first_point = vert_a.unwrap();

        let emit_shape = |a: &[f64; 2], b: &[f64; 2]|
            println!(
                "{}->addShape(PhysicsShapeEdgeSegment::create(Vec2({:.10}f, {:.10}f), Vec2({:.10}f, {:.10}f)));",
                Emitter::varname(id, physicsbody),
                a[0], a[1],
                b[0], b[1]
            );

        while let (Some(a), Some(b)) = (vert_a, vert_b) {
            emit_shape(&a, &b);

            vert_a = vert_b;
            vert_b = verts.next();
        }

        if self.verts.len() > 2 {
            emit_shape(&vert_a.unwrap(), &first_point);
        }
    }
}


pub struct Circle {
    cx: f64,
    cy: f64,
    r:  f64,
    color: Color
}

impl Shape for Circle {
    fn emit_graphics(&self, id: &str, drawnode: &str, color: Option<&str>) {
        println!("// Circle for {}", id);
        // arguments: center, radius, angle, segments, color
        println!(
            "{}->drawSolidCircle(Vec2({:.10}f,{:.10}f), {:.10}f, 0.0f, 20, {});",
            Emitter::varname(id, drawnode),
            self.cx, self.cy, self.r,
            color.unwrap_or(&self.color.emit())
        );
    }

    fn emit_physics(&self, _id: &str, _physicsbody: &str) {
        panic!("Can't do physics for circle yet (no use case)");
    }
}

pub struct Rect {
    x: f64,
    y: f64,
    w: f64,
    h: f64,
    color: Color
}

impl Shape for Rect {
    fn emit_graphics(&self, id: &str, drawnode: &str, color: Option<&str>) {
        println!("// Rect for {}", id);
        // arguments: origin, destination, color
        println!(
            "{}->drawSolidRect(Vec2({:.10}f,{:.10}f), Vec2({:.10}f,{:.10}f), {});",
            Emitter::varname(id, drawnode),
            self.x,        self.y,
            self.x+self.w, self.y+self.h,
            color.unwrap_or(&self.color.emit())
        );
    }

    fn emit_physics(&self, id: &str, physicsbody: &str) {
        println!("// Physics for {}", id);

        let emit_shape = |a: &[f64; 2], b: &[f64; 2]|
            println!(
                "{}->addShape(PhysicsShapeEdgeSegment::create(Vec2({:.10}f, {:.10}f), Vec2({:.10}f, {:.10}f)));",
                Emitter::varname(id, physicsbody),
                a[0], a[1],
                b[0], b[1]
            );

        // bottom-left to bottom-right
        emit_shape(&[self.x, self.y], &[self.x+self.w, self.y]);
        // bottom-right to top-right
        emit_shape(&[self.x+self.w, self.y], &[self.x+self.w, self.y+self.h]);
        // top-right to top-left
        emit_shape(&[self.x+self.w, self.y+self.h], &[self.x, self.y+self.h]);
        // top-left to bottom-left
        emit_shape(&[self.x, self.y+self.h], &[self.x, self.y]);
    }
}

/// The emitter itself. Holds onto shapes relative to their id.
pub struct Emitter {
    origin: Option<[f64; 2]>,
    shapes: BTreeMap<String, Box<Shape>>
}

impl Emitter {
    pub fn new() -> Emitter {
        Emitter {
            origin: None,
            shapes: BTreeMap::new()
        }
    }

    ///
    /// Emit a single shape to stdout.
    /// Returns true if a shape under `id` was found and emitted.
    /// Returns false if there was no shape under `id`.
    ///
    pub fn emit(&self, id: &str, drawnode: Option<&str>, physicsbody: Option<&str>, color: Option<&str>) -> bool {
        match self.shapes.get(id) {
            Some(shape) =>{
                if let Some(dn) = drawnode {
                    shape.emit_graphics(id, dn, color);
                }
                if let Some(pb) = physicsbody {
                    shape.emit_physics(id, pb);
                }
                true
            }
            None => false
        }
    }

    ///
    /// Emit all shapes to stdout.
    ///
    pub fn emit_all(&self, drawnode: Option<&str>, physicsbody: Option<&str>, color: Option<&str>) {
        for (ref id, ref shape) in &self.shapes {
            if let Some(dn) = drawnode {
                shape.emit_graphics(id, dn, color);
            }
            if let Some(pb) = physicsbody {
                shape.emit_physics(id, pb);
            }
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
            "rect"   => Box::new(self.parse_rect(attributes)),
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

        let ref style = attributes.iter().find(|ref a| a.name.local_name == "style")
            .expect("Path lacking 'style' attribute")
            .value;

        Polygon {
            triangles: polyfill::triangle_indices(&verts, polyfill::TANGENTIAL),
            verts: verts,
            color: Emitter::parse_color_from_style(style).unwrap_or(Color::black())
        }
    }

    ///
    /// Parse a circle with center point based on `self.origin`.
    ///
    pub fn parse_circle(&mut self, attributes: &Vec<xml::attribute::OwnedAttribute>) -> Circle {
        //                 cx    cy    r    color
        let mut params = (None, None, None, None);

        for ref attr in attributes {
            let name: &str = &attr.name.local_name;
            match name {
                "cx" => params.0 = Some(attr.value.clone()),
                "cy" => params.1 = Some(attr.value.clone()),
                "r"  => params.2 = Some(attr.value.clone()),
                "style"  => params.3 = Some(attr.value.clone()),
                _ => {}
            }

            if let (Some(cx), Some(cy), Some(r), Some(style)) = params {
                let mut cxy = [0.0, 0.0];
                self.assign_position_from_origin(&mut cxy, &cx, &cy);
                return Circle {
                    cx: cxy[0], cy: cxy[1], r: parse_float!(&r),
                    color: Emitter::parse_color_from_style(&style).unwrap_or(Color::black())
                };
            }
        }
        panic!("Invalid circle");
    }

    ///
    /// Parse a rect with origin at the bottom right (??)
    ///
    pub fn parse_rect(&mut self, attributes: &Vec<xml::attribute::OwnedAttribute>) -> Rect {
        //                 x,    y,    w,    h,   style
        let mut params = (None, None, None, None, None);

        for ref attr in attributes {
            let name: &str = &attr.name.local_name;
            match name {
                "x"      => params.0 = Some(attr.value.clone()),
                "y"      => params.1 = Some(attr.value.clone()),
                "width"  => params.2 = Some(attr.value.clone()),
                "height" => params.3 = Some(attr.value.clone()),
                "style"  => params.4 = Some(attr.value.clone()),
                _ => {}
            }

            if let (Some(x), Some(y), Some(w), Some(h), Some(style)) = params {
                let mut xy = [0.0, 0.0];
                self.assign_position_from_origin(&mut xy, &x, &y);
                let fw = parse_float!(&w);
                let fh = parse_float!(&h);
                return Rect {
                    x: xy[0], y: xy[1] - fh,
                    w: fw,    h: fh,
                    color: Emitter::parse_color_from_style(&style).unwrap_or(Color::black())
                }
            }
        }
        panic!("Invalid rect");
    }

    // =========== Private helper functions ===========

    fn parse_color_from_style(style: &str) -> Option<Color> {
        let exprs = style.split(';');
        for ref expr in exprs {
            let mut keyval = expr.split(':');
            let key = keyval.next().expect("Invalid style entry");

            if key == "fill" {
                let val = keyval.next().expect("Invalid style entry");
                return Some(Color::parse(val));
            }
        }
        None
    }

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
