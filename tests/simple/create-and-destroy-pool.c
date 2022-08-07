#include <pmalloc/pmalloc.h>

int main(void) {
    pmalloc_pool_t *pool = pmalloc_create_pool();
    pmalloc_destroy_pool(pool);
    return 0;
}
