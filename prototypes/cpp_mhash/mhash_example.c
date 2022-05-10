 #include <mhash.h>
 #include <stdio.h>
 #include <stdlib.h>

 int main(void) 
 {
        int i;
        MHASH td;
        unsigned char buffer;
        unsigned char *hash;

        td = mhash_init(MHASH_SHA256);

        if (td == MHASH_FAILED) exit(1);

        while (fread(&buffer, 1, 1, stdin) == 1) {
                mhash(td, &buffer, 1);
        }

        hash = mhash_end(td);

        printf("Hash:");
        for (i = 0; i < mhash_get_block_size(MHASH_SHA256); i++) {
                printf("%.2x", hash[i]);
        }
        printf("\n");

        exit(0);
 }
