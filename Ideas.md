# Cursor control

The X, Y and Z keys will lock the movement of the 3D cursor to that axis.
Shift + X, Y or Z will lock the movement of the 3D cursor to the plane that axis is no part of.

H, V and D also lock to the x y and z axes, but matching the view plane as closely. H for horizontal, V for vertical, D for depth.

## Selection

Combing the above with dragging the left mouse button in selection mode allows for selecting 1D/2D areas of blocks. This selection can then be expanded in the remaining direction through numerical input.

R selects the rectangle selection tool. E selects the ellipse selection tool.

Tab switches boolean selection modes:

- Select (new)
- Add (old || new)
- Subtract (old && !new)
- Multiply (old && new)

The %-key toggles random. Pi

# Camera control

Dragging the middle mouse button rotates around the position of the 3D cursor.

Combining the middle mouse button with the Ctrl key zooms to and from the 3D cursor.

Shift and the middle mouse button pans in the view plane.

# Block picking

Right mouse button picks the block at the 3D cursor. With that rotation.

B brings up the block picker. When held down it will pick the block that the cursor is hovering over. Else it will wait for a left mouse click to pick a block.

Shift + B will bring up the palette picker. Allowing you to preselect from the many blocks you may have at your disposal.

Ctrl + B will bring up the block editor that allows you to create palettes, add blocks, clone blocks and combine blocks