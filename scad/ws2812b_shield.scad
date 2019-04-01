module prism(l, w, h){
    polyhedron(
        points=[[0,0,0], [l,0,0], [l,w,0], [0,w,0], [0,w,h], [l,w,h]],
        faces=[[0,1,2,3],[5,4,3,2],[0,4,5,1],[0,3,4],[5,2,1]]
    );
}

module trapezoid(l, w, h, space) {
    union() {
        prism(l, w, h);
        translate([0,w,0])
            cube([l, space, h]);
        translate([0, w*2 + space,0])
            mirror([0, 1, 0])
                prism(l, w, h);
    }
}

module led_shield(l, w, h, space, inner_space, inner_len = 1.0) {
    border = 1;
    difference() {
        trapezoid(l, w, h, space);
        translate([0, (w+space/2) - (border + inner_space/2),0])
            trapezoid(l*inner_len, border, h * 0.5, inner_space);
    }
}

module led_shield_end(l, w, h, space, inner_space) {
    cut_length = l * 0.5;
    cut_width = w * 2.0 + space;

    difference() {
        led_shield(l, w, h, space, inner_space, 0.55);
        rotate([0, 180, -90])
            translate([0, cut_length,-h])
                prism(cut_width, cut_length, h);
    }
}

translate([-25, 0, 0])
    led_shield(20, 16.50, 6, 11, 11);

led_shield_end(20, 16.50, 6, 11, 11);
