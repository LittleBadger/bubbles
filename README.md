# bubbles

See [here](https://littlebadger.github.io/bblwebgl.html) for the webgl demo.

Two dimensional bubbles satisfy a 2D version of Plateau's laws:
 * Every bubble wall is an arc of a circle with (signed) radius inversely proportional to the difference of pressures on either side. 
 * Bubble walls meet at junctions of three, with a 120 degree angle between each pair of walls.

For example, a solitary bubble's radius is inversely proportional to it's pressure. Moreover, two bubbles of radii r1 and r2 that are in contact must be a distance \sqrt{ r1^2 + r2^2 - r1 r2 } apart, and are seperated by a wall of radius r1 r2 / ( r1 + r2 ), which curves into the bigger bubble. For three(or more) bubbles, with some effort you can check that if every pair of bubbles i and j are the distance \sqrt{ri^2 + rj^2 - ri rj} apart, then the Plateau's laws are satisfied; in particular, the three walls seperating pairs of bubbles intersect at one point, with angle 120 degrees. 

A bubble cluster satisfying these laws can be drawn as a Voronoi/power diagram, with the function f_i(p) = 1/ri d(p,ci) - ri, where p is a point in the plane, and d(p,ci) is the distance from p to the center of bubble. In other words, a point p is inside bubble i if f_i(p) < 0 and f_i(p) < f_j(p) for all other bubbles j; if f_i(p) = f_j(p) then p is on the wall ij between bubble i and j.

So a very simple cartoon simulation is as follows: put a force with the correct rest distance between every pair of bubbles in contact, and when the bubbles collide increase the radius a bit to conserve volume of the bubble. Then draw the Voronoi diagram, per pixel with the fragment shader, and simultaneously compute the adjacency matrix of bubbles in contact: if a pixel finds himself on the bubble wall ij then he writes 1 to that pixel in an image using image load/store.

Thanks to my friends at TU, especially Niklas and Jan, for enthusiastically allowing me to waste an afternoon with this.


