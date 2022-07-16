#include "apilib.h"

void HariMain(void)
{
    /* 超頭悪い円周率計算 pi = 4 arctan(1) = 4(1-1/3+1/5-1/7+1/9-...) より */
    double s = 0.0;
    int i, d;
    for (i = 1; i < 500000000; i += 4) { /* 5億くらいまでやらないと値がまともにならない */
        s += 1.0 / i - 1.0 / (i + 2);
    }
    s *= 4.0;

    for (i = 0; i < 15; i++) { /* 10未満の正の数を表示 */
        d = (int) s;
        api_putchar('0' + d);
        s = (s - d) * 10.0;
        if (i == 0) {
            api_putchar('.');
        }
    }
    api_putchar('\n');
    api_end();
}
