use polyfill;

use std::str::FromStr;

macro_rules! parse_float {
    ($s:expr) => (f64::from_str($s).expect("Failed to parse float"))
}

pub struct Emitter {
    origin: Option<[f64; 2]>
}

impl Emitter {
    pub fn new() -> Emitter {
        Emitter { origin: None }
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

    ///
    /// Emits C++ code that draws a Cocos2dx circle.
    ///
    pub fn emit_circle(&mut self, scx: &str, scy: &str, sr: &str, id: &str, drawnode: &str, color: &str) {
        let mut pos = [0.0, 0.0];
        self.assign_position_from_origin(&mut pos, scx, scy);

        println!("// Circle for {}", id);
        // arguments: center, radius, angle, segments, color
        println!(
            "{}->drawSolidCircle(Vec2({:.10}f,{:.10}f), {:.10}f, 0.0f, 20, {});",
            drawnode,
            pos[0], pos[1],
            sr,
            color
        );
    }

    ///
    /// Parses the "d" attribute of an Inkscape SVG <path> tag and emits C++
    /// code that draws Cocos2dx triangles that make up the shape of the path.
    ///
    pub fn emit_path(&mut self, d: &str, id: &str, drawnode: &str, color: &str) {
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

        // Triangulation and c++ code emission
        println!("// Triangles for {}", id);
        let triangles = polyfill::triangle_indices(&verts, polyfill::TANGENTIAL);
        for t in triangles {
            println!(
                "{}->drawTriangle(Vec2({:.10}f,{:.10}f), Vec2({:.10}f,{:.10}f), Vec2({:.10}f,{:.10}f), {});",
                drawnode,
                verts[t[0]][0], verts[t[0]][1],
                verts[t[1]][0], verts[t[1]][1],
                verts[t[2]][0], verts[t[2]][1],
                color
            );
        }
    }
}
