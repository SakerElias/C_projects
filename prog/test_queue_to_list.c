#include <pyas/queue.h>
#include <pyas/list.h>
#include <pyas/charset.h>

int main() {
    queue_t file = queue_new();
    charset_t a= charset_initialize();
    a = create_cset(a, "ab", 0, 1);
    charset_t b= charset_initialize();
    b = create_cset(b, "cd", 0, 1);
    file = enqueue(file, a);
    file = enqueue(file, b);
    list_t l = queue_to_list(file);
    list_print(l, charset_print_wrapper);
    return 1;
}
