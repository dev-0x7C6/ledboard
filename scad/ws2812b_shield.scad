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

module led_shield(l, w, h, space, inner_space) {
    difference() {
        trapezoid(l, w, h, space);
        translate([0, (w+space/2) - (w*0.2 + inner_space/2),0])
            trapezoid(l, w * 0.2, h * 0.6, inner_space);
    }
}


   
led_shield(10, 16.50, 5, 11, 11);