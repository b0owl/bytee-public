### Init
**`Allocator a;`** defines the Allocator object.  
**`create_pointers(int size)`**  Defines a memory pool to be handed out to your program at your discretion. Creating more pointers with this will set previous ones to nullptr. (Do note, you can create more `Allocator` objects and draw to that instead of overriding the previous in the event you run out of space in the pool, this form of handling prevents over-using ram, and keeps performance to an absolute maximum)

### Drawing
**`draw_struct(void** ptr, int count)`**  Takes an array of void* pointers and a count, and casts each to a DrawData pointer, then renders with `glBegin(GL_TRIANGLE_FAN)`. Pass `count` as length of `void**`

### Deconstruction
On deconstruction all pointers will be automatically freed.