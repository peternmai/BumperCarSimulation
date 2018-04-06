# Bumper Car Simulation - *Modern OpenGL Demonstration*

This was a fun modern OpenGL demonstration showing the bountiful possibilities
that could happen when you bring computer graphics and stressed undergraduate
students together! Wait.. I think I said that wrong.. computer graphics and
*creativity*!

Anywho... below is a short video of what we made! :)

[![Demonstration on YouTube](/Doc/Images/Preview.png)](https://www.youtube.com/watch?v=LGYbFkSXVtw)

*Link: [https://www.youtube.com/watch?v=LGYbFkSXVtw](https://www.youtube.com/watch?v=LGYbFkSXVtw)*

## Features

#### Particle Effects

Why particles? Because now the simulation looks way more festive than it should
be! We have a particle engine that generates roughly 500 particles through
instancing. Each particle moves/lives/dies independently of one another. A
random number generator is used to determine the direction the particle
is moving towards, as well as how long it'll keep that direction. The particle
will also be assigned a random life span. When a particle is just created,
it'll appear bigger and is less transparent. As the particle approaches the end
of its life, the size of it decreases and begins to fade out. All in all,
we can just accept it's a giant confetti party.

#### Phong Shading and Shadow Mapping

Next up, we added shadows and phong illumination to the simulation. This makes
the graphic in the game even more realistic. So realistic you have to believe
that a bumper car arena hoisted in the air by a giant blimp is possible!
Because it is o_o. With regards to creating shadows, we needed to render the
shadow in two pass. In the first pass, we created a depth map from the
perspective of the light source (sun). In the second pass, we will render
all our objects as usual. But the key here, is we would use the depth map
created from the first pass to determine if a pixel is in the shadow or not.
If it is in the shadow, we would apply the shadow (darken the area) using the
Poisson sampling technique. This softens the edges of the shadow to make it
look more realistic.

#### Linear Fog

How could we make driving a bumper car around in the middle of the sky even
more fun? Add fog. A lot of fog! It's almost like driving with your eyes closed!
How fun is that? JK. Don't do that. In here, we basically have two planes; a
near plane at the camera (0% fog) and a far plane away from the camera (100% fog).
The idea here is we would determine how far the object is from the camera
and calculate where it sits respectively within this near and far plane. We would
then mix the color of gray with the object's color accordingly based on how far
the object is from the camera. This in a sense gives the impression that the
object is in a foggy environment.

#### Collision Detection

Finally, we can't possibly have a bumper car simulation if we can't detect
when two cars have collided. Therefore, we created tightly bounded boxes around
each of the vehicle and checked when any portion of the box is intersecting the box
of another vehicle. When two or more boxes overlap with each other, we know that the
vehicles have collided. Once a collision is detected, we need to determine how the
vehicles should respond. We do this by determining which vehicle is at fault. Which
vehicle is the one hitting the other. This is done by looking at the vehicle's position
and direction to determine where it's heading. Once this is determined, our simulation
would change the direction of the vehicle who is at fault so that they're no longer
on a collision course with one another.

---

*This was done as part of our final project for UCSD CSE 167 and was posted
with permission from Professor Ben Ochoa.*
