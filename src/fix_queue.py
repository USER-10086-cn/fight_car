
with open(r'd:\fight_csr\project\src\queue.c', 'r', encoding='utf-8') as f:
    content = f.read()

# Fix 1: Move declaration from before { to inside function body
content = content.replace(
    'queue_handle_t queue_create(uint16_t item_size)\n    queue_handle_t queue;\n{',
    'queue_handle_t queue_create(uint16_t item_size)\n{\n    queue_handle_t queue;')

content = content.replace(
    'void queue_delete(queue_handle_t queue)\n    queue_node_t* current;\n{',
    'void queue_delete(queue_handle_t queue)\n{\n    queue_node_t* current;')

content = content.replace(
    'bool queue_send(queue_handle_t queue, const void* item)\n    queue_node_t* new_node;\n{',
    'bool queue_send(queue_handle_t queue, const void* item)\n{\n    queue_node_t* new_node;')

content = content.replace(
    'bool queue_receive(queue_handle_t queue, void* item)\n    queue_node_t* node;\n{',
    'bool queue_receive(queue_handle_t queue, void* item)\n{\n    queue_node_t* node;')

# Fix 2: Fix indentation of assignment lines (8 spaces -> 4 spaces)
content = content.replace('\n        queue = ', '\n    queue = ')
content = content.replace('\n        current = ', '\n    current = ')
content = content.replace('\n        new_node = ', '\n    new_node = ')
content = content.replace('\n        node = ', '\n    node = ')

with open(r'd:\fight_csr\project\src\queue.c', 'w', encoding='utf-8') as f:
    f.write(content)

print('Fixed!')
