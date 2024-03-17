# Platformer

## ToDo
- [x] render a dude and have him move
- [x] able to get render a portion of the texture
- [x] render "selectable tiles"
- [x] render empty map
- [x] render selected tile
- [x] able to place a tile
- [x] able to select what "category" you want to place
- [x] able to select background
- [x] able to place enemies
- [x] create/choose markers
    - [x] spawn point
    - [x] exit point
        Just the doors?
- [x] "recreate" the character and tile maps
- [x] able to select spawn point
- [x] able to select exit point?
- [x] figure out data format (json?) for map
    {
        "width": X,
        "height": Y, 
        "tiles": [],
        "background": z,
        "enemies": []
    }
- [x] save map
- [x] load map
- [x] basic movement, gravity and collision
- [x] differentiate between ground and other collidedable objects
- [x] spawn at correct position and end game at correct position
- [x] figure out better way to handle map
- [x] figure out a mapping from data to tileMapping so that it can change
    - [x] i guess do the same for characters and backgrounds
- [x] figure out animation format

## Game Things
### Enemies
- [x] Spawn them
- [ ] Have them move
- [ ] Have them kill you :)


### Randoms
I need some sort of concept of a entity that animates based on a function
    Can be an index into a table of functions
    When we calculate this we also might want to do some other sort of interaction
Also need to compose some sort of things as well


### Interaction with ui
- [ ] fix everything from test map
    - [ ] the pole thingy that move when you move over it
    - [ ] the bouncing thing
    - [ ] ladders
    - [ ] gather emeralds/key
    - [ ] take dmg from spikes
    - [x] See if you can animate the water
    - [x] animate flag
    - [ ] Have x3 clouds move
### Random
- [ ] have some sort of hp idea and respawn rather then die
- [ ] fix some sort of background in general
- [ ] fix bounds
- [ ] actually do things with string library? :)
- [ ] actually log stuff? :)



## Cleanup
cleanup what's common and whats not


## Editor
