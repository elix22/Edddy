# Edddy

## Cursor control

The 3D cursor can be controlled with the mouse or arrow keys. By default the mouse will raycast to the first block it encounters on th active layer.
The arrow keys will select along the XZ-plane by default, with **\[** and **\]** moving the 3D cursor along the remaining axis.

The **X**, **Y** and **Z** keys will lock the movement of the 3D cursor to that axis.
**Shift + X**, **Y** or **Z** will lock the movement of the 3D cursor to the plane that axis is no part of.

**H**, **V** and **D** also lock to the x y and z axes, but matching the view plane as closely. **H** for horizontal, **V** for vertical, **D** for depth.

Pressing the same key as the current locking will release the lock.

## Selection

Combing the above with dragging the **left mouse button** in selection mode allows for selecting 1D/2D areas of blocks. This selection can then be expanded in the remaining direction through **numerical input**.

Alternatively the **arrow keys** can be combined with holding down **Shift** to select an area.

**R** selects the rectangle selection tool. **E** selects the ellipse selection tool.

Tab switches boolean selection modes:

- Standard (new)
- Add (old || new)
- Subtract (old && !new)
- Multiply (old && new)

Clicking with empty selection in Substract or Multiply mode will switch back to Standard select mode.

**A** toggles selected for all locked axes. If no axes are locked or 

## Camera control

Dragging the **middle mouse button** rotates around the position of the 3D cursor.

Combining the **middle mouse button** with the **Ctrl** key zooms to and from the 3D cursor.

**Shift** and the **middle mouse button** pans in the view plane.

## Block picking

Right mouse button (drag or click) creates a stamp using the last or currently used selection tool.

**B** brings up the current block palette. When **B** is held down it will pick the block that the cursor is hovering over. Else it will wait for a left mouse click to pick a block.

**Ctrl + B** will bring up the block editor that allows you to create and order palettes and blocks.

* Next block: **.**
* Previous block: **,**
* Next palette: **>**  
* Previous palette: **<**

## Rotating block

**(** and **)** will rotate the picked block or stamp 90 degrees (ccw and cw resp.) along either the only locked or only non-locked axis. (Y-axis by default)

## Putting blocks

**Left click** places the block or stamp. Combined with **Shift** this will draw a line from the previously put coordinates and the current.

## Map properties

The map properties are accessed with **Ctrl+M**

Here you can change the size of the map, the grid and the block offset.

## Other tools and shortkeys:

+ **F1** or **Alt+H**: Help
+ **Enter**/**Space**: Alternative for left mouse click.
+ **Q**: Eraser
+ **%**: Random, picking a random tile from all the tiles in a stamp every time a block is put.
+ **O**: Type select
+ **Backspace**: Select previous tool
+ **#** : Toggles between grid and free placement

+ **Ctrl+R**: Replace or swap block types

-----------------------

+ **Delete**: Remove selected blocks
+ **Shift+Delete**: Clear along locked axes. When this area is already empty weld the neighboring planes.
+ **Ctrl+Delete**: Zealous crop
+ **Shift+Insert**: Add an empty plane at 3D cursor, thickness can be increased with numerical keys. This changes the size of the map.

-----------------------

+ **Ctrl+Z**: Undo
+ **Ctrl+Shift+Z** / **Ctrl+Y** : Redo
+ **Ctrl+Alt+Z** : Selection undo
+ **Ctrl+Shift+Alt+Z** : Selection redo

+ **Ctrl+Q**: Quit

## Block definition

A block has a model and zero or more materials.

## Layers

Layers allow for multiple blocks at the same coordinates. Ctrl combined with a number switches to that layer. Shift + Ctrl toggles that layers visibility. **\`** toggles the visibility of all layers.

## EMP Format

EMP stands for Edddy Map