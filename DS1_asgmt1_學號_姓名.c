#include <stdio.h>
/*
請依需求自由加入標頭檔
*/
#include <stdlib.h>   /* malloc / free / strtoll / exit                  */
#include <string.h>   /* strtok / strchr                                 */
#include <errno.h>    /* errno / ERANGE：判斷 strtoll 轉換是否超出範圍    */
#include <math.h>     /* sqrtl / powl / roundl：公式法需要的數學函式      */
#include <time.h>     /* clock()：量測各方法執行時間，用來比較效率        */

/*
 * MAX_N：允許輸入的最大項數
 * --------------------------------------------------------------------
 * 本程式使用 long long（64 位元有號整數）存放費氏數，
 * 其最大值為 2^63 - 1 = 9,223,372,036,854,775,807。
 *   F(92) = 7,540,113,804,746,346,429  → 仍可存下
 *   F(93) = 12,200,160,415,121,876,738 → 超過上限，會溢位
 * 所以最多只能計算到第 92 項。
 */
#define MAX_N 92

/* 使用者每次輸入一行文字的緩衝區大小 */
#define BUF_SIZE 256

/* 遞迴法的「函式呼叫次數」計數器，用來說明遞迴為何效率差 */
static unsigned long long recursion_calls = 0;


/* ====================================================================
 * 輔助函式：印出一個數值，並在數值之間加上逗號
 * --------------------------------------------------------------------
 * 參數：k      目前是第幾項
 *       n      起始項（第 n 項前面不印逗號）
 *       value  F(k) 的值
 * 輸出格式範例：Fibonacci(2, 5) = 1, 2, 3, 5
 * ==================================================================== */
static void print_term(int k, int n, long long value) {
    if (k != n) printf(", ");
    printf("%lld", value);
}


/*
以下四個函式名稱請勿更動
型別、使用方式、參數列、回傳型別、輸出方式、所需輸入引數及變數請自行設計
*/

/* ====================================================================
 * 1. 迭代法 (Iterative)
 * --------------------------------------------------------------------
 * 參數：n  起始項、m  結束項 (0 <= n <= m)
 * 輸出：依序印出 F(n), F(n+1), ..., F(m)
 *
 * 原理：
 *   F(0) = 0、F(1) = 1、F(i) = F(i-1) + F(i-2)
 *   只要記住「前兩項」，用一個迴圈從第 0 項一路往後推到第 m 項，
 *   推到第 n 項之後，每算出一項就順便印出來。
 *
 * 效率：
 *   時間複雜度 O(m)  —— 只需跑一次迴圈，每項只算一次
 *   空間複雜度 O(1)  —— 只用到 prev、curr、next 三個變數
 * ==================================================================== */
void Fibonacci_Iterative(int n, int m) {
    long long prev = 0;   /* 代表 F(i)  ，一開始是 F(0) = 0 */
    long long curr = 1;   /* 代表 F(i+1)，一開始是 F(1) = 1 */
    long long next;
    int i;

    printf("Fibonacci(%d, %d) = ", n, m);

    for (i = 0; i <= m; i++) {
        /* 到達起始項 n 之後才開始印 */
        if (i >= n) print_term(i, n, prev);

        /* 把「前兩項」往後挪一格：(F(i), F(i+1)) → (F(i+1), F(i+2)) */
        next = prev + curr;
        prev = curr;
        curr = next;
    }
    printf("\n");
}

/* ====================================================================
 * 遞迴法使用的輔助函式：以遞迴計算單一項 F(k)
 * --------------------------------------------------------------------
 * 直接把數學定義翻譯成程式：
 *   F(k) = F(k-1) + F(k-2)，終止條件 F(0)=0、F(1)=1
 * 同一個子問題會被重複計算非常多次，
 * 例如算 F(5) 時，F(3) 被算 2 次、F(2) 被算 3 次……
 * 所以呼叫次數會隨 k 呈指數成長，約 O(1.618^k)。
 * ==================================================================== */
static long long fib_recursive(int k) {
    recursion_calls++;        /* 每進入一次函式就計數一次 */

    /* 終止條件 (base case)：沒有它，遞迴會無限呼叫下去 */
    if (k == 0) return 0;
    if (k == 1) return 1;

    /* 遞迴關係：把大問題拆成兩個較小的子問題 */
    return fib_recursive(k - 1) + fib_recursive(k - 2);
}

/* ====================================================================
 * 2. 遞迴法 (Recursion)
 * --------------------------------------------------------------------
 * 參數：n  目前要印的項、m  結束項
 *       first  起始項（用來判斷要不要印逗號，外部呼叫時傳入與 n 相同的值）
 * 輸出：依序印出 F(n), ..., F(m)
 *
 * 原理：
 *   這個函式本身也用遞迴處理「n 到 m 的範圍」：
 *     先印出 F(n)（由 fib_recursive 遞迴算出），
 *     再呼叫自己處理剩下的範圍 (n+1 ~ m)；
 *     當 n > m 時代表範圍已印完，即為終止條件。
 *
 * 效率：
 *   時間複雜度 O(2^m)（更精確約 O(1.618^m)）
 *   —— 每一項都要從頭重算，且單項計算本身就是指數級
 *   空間複雜度 O(m)  —— 遞迴呼叫堆疊的最大深度
 *   m 大約超過 40 之後，執行時間會明顯變長。
 * ==================================================================== */
void Fibonacci_Recursion(int n, int m, int first) {
    /* 終止條件：範圍已經全部印完 */
    if (n > m) {
        printf("\n");
        return;
    }

    if (n == first) printf("Fibonacci(%d, %d) = ", first, m);
    print_term(n, first, fib_recursive(n));   /* 印出 F(n) */

    Fibonacci_Recursion(n + 1, m, first);     /* 遞迴處理下一項 */
}

/* ====================================================================
 * 3. 陣列法 (Array / 動態規劃 Dynamic Programming)
 * --------------------------------------------------------------------
 * 參數：n  起始項、m  結束項 (0 <= n <= m)
 * 輸出：依序印出 F(n), ..., F(m)
 *
 * 原理：
 *   建一個長度 m+1 的陣列 fib[]，fib[i] 存放 F(i)。
 *   先填入 fib[0]=0、fib[1]=1，再由小到大依序
 *   fib[i] = fib[i-1] + fib[i-2]，每一項只算一次並存起來，
 *   最後直接從陣列取出 fib[n] ~ fib[m] 印出即可（查表）。
 *
 * 效率：
 *   時間複雜度 O(m)
 *   空間複雜度 O(m)  —— 需要一整個陣列存放所有項
 *   （與迭代法相比多花了記憶體，但保留了所有中間結果，可重複查詢）
 * ==================================================================== */
void Fibonacci_Array(int n, int m) {
    long long *fib;           /* 指向動態配置的陣列 */
    int i;

    /* 依 m 的大小動態配置陣列；至少配 2 格以便放 fib[0]、fib[1] */
    fib = (long long *)malloc(sizeof(long long) * (m < 2 ? 2 : m + 1));
    if (fib == NULL) {
        printf("記憶體配置失敗！\n");
        return;
    }

    /* 初始值（數學定義） */
    fib[0] = 0;
    fib[1] = 1;

    /* 由小到大填表：每一格都由前兩格相加而來 */
    for (i = 2; i <= m; i++) {
        fib[i] = fib[i - 1] + fib[i - 2];
    }

    /* 查表輸出第 n ~ m 項 */
    printf("Fibonacci(%d, %d) = ", n, m);
    for (i = n; i <= m; i++) {
        print_term(i, n, fib[i]);
    }
    printf("\n");

    free(fib);                /* 用完記得釋放記憶體，避免 memory leak */
}

/* ====================================================================
 * 4. 公式法 (Binet's Formula 比內公式)
 * --------------------------------------------------------------------
 * 參數：n  起始項、m  結束項 (0 <= n <= m)
 * 輸出：依序印出 F(n), ..., F(m)
 *
 * 原理：
 *   費氏數列有封閉形式（一般項）公式：
 *
 *            1    [ (1 + √5)^k     (1 - √5)^k ]
 *   F(k) = ---- × [ (-------)   -  (-------)  ]
 *           √5    [ (   2   )      (   2   )  ]
 *
 *   其中 φ = (1+√5)/2 ≈ 1.618 為黃金比例，ψ = (1-√5)/2 ≈ -0.618。
 *   每一項都直接代公式算出，不需要知道前兩項。
 *
 * 效率：
 *   單項時間複雜度 O(1)（若把 pow 的運算算進去則約 O(log k)），
 *   印出 n~m 共 (m-n+1) 項，總時間為 O(m-n+1)。
 *   空間複雜度 O(1)。
 *
 * 注意（浮點數精度問題）：
 *   √5 是無理數，電腦只能以近似值儲存，k 越大誤差越大。
 *   這裡使用 long double 並以 roundl() 四捨五入到最近整數來降低誤差：
 *   - 若改用 double，大約從 F(71) 開始就會算錯；
 *   - long double 在 gcc（如 Dev-C++、Code::Blocks）上可正確算到 F(92)，
 *     但在 Visual Studio (MSVC) 上 long double 等同 double，精度較差。
 * ==================================================================== */
void Fibonacci_Formula(int n, int m) {
    long double sqrt5 = sqrtl(5.0L);              /* √5           */
    long double phi   = (1.0L + sqrt5) / 2.0L;    /* φ 黃金比例   */
    long double psi   = (1.0L - sqrt5) / 2.0L;    /* ψ 共軛根     */
    long double value;
    int k;

    printf("Fibonacci(%d, %d) = ", n, m);
    for (k = n; k <= m; k++) {
        /* 代入比內公式，並四捨五入消除浮點誤差 */
        value = (powl(phi, k) - powl(psi, k)) / sqrt5;
        print_term(k, n, (long long)roundl(value));
    }
    printf("\n");
}


/* ====================================================================
 * 輔助函式：解析一個字串是否為「合法的整數」
 * --------------------------------------------------------------------
 * 參數：str  要解析的字串
 *       out  解析成功時，把結果寫入 *out
 * 回傳：1 = 是合法整數；0 = 不是整數（含小數、英文字母、符號等）
 *
 * 使用 strtoll 而不用 scanf("%d")，原因是 scanf 遇到 "3.5" 會只讀 3、
 * 遇到 "abc" 會卡住緩衝區造成無窮迴圈，無法精確判斷錯誤種類。
 * ==================================================================== */
static int parse_integer(const char *str, long long *out) {
    char *end;
    long long value;

    errno = 0;
    value = strtoll(str, &end, 10);   /* 以十進位轉換 */

    if (end == str)   return 0;       /* 完全沒有讀到數字，如 "abc"     */
    if (*end != '\0') return 0;       /* 數字後面還有其他字元，如 "3.5" */
    if (errno == ERANGE) {            /* 數字大到 long long 都放不下   */
        value = (value > 0) ? MAX_N + 1 : -1;   /* 交給後續的範圍檢查處理 */
    }
    *out = value;
    return 1;
}

/* ====================================================================
 * 輔助函式：讀取並驗證使用者輸入的「一個數字」
 * --------------------------------------------------------------------
 * 參數：prompt  顯示給使用者的提示文字
 * 回傳：通過驗證的項數 (0 <= 值 <= MAX_N)
 *
 * 針對每一種違規情況顯示不同的錯誤訊息，並要求重新輸入：
 *   (1) 沒有輸入任何內容（空白行）
 *   (2) 一次輸入了多個數字
 *   (3) 輸入的不是整數（小數、英文字母、特殊符號……）
 *   (4) 輸入負數 —— 費氏數列的項數在數學上定義於非負整數，
 *                   F(0) 為第零項，不存在「第 -1 項」
 *   (5) 輸入超過 MAX_N —— 結果會超出 long long 範圍而溢位
 * ==================================================================== */
static int read_valid_number(const char *prompt) {
    char line[BUF_SIZE];
    char *token, *extra;
    long long value;

    while (1) {
        printf("%s", prompt);

        /* 讀一整行；若遇到 EOF（例如 Ctrl+Z / Ctrl+D）則結束程式 */
        if (fgets(line, sizeof(line), stdin) == NULL) {
            printf("\n未讀取到輸入，程式結束。\n");
            exit(1);
        }

        /* 若一行太長而沒讀完，把剩下的字元清掉，避免影響下一次輸入 */
        if (strchr(line, '\n') == NULL) {
            int ch;
            while ((ch = getchar()) != '\n' && ch != EOF) { }
        }

        /* 以空白、Tab、換行切出第一個 token */
        token = strtok(line, " \t\r\n");

        /* (1) 空白輸入 */
        if (token == NULL) {
            printf("[錯誤] 沒有輸入任何內容，請輸入一個整數。\n\n");
            continue;
        }

        /* (2) 一次輸入多個值 */
        extra = strtok(NULL, " \t\r\n");
        if (extra != NULL) {
            printf("[錯誤] 一次只能輸入一個數字，請重新輸入。\n\n");
            continue;
        }

        /* (3) 不是整數 */
        if (!parse_integer(token, &value)) {
            printf("[錯誤] \"%s\" 不是整數！費氏數列的項數必須為整數，"
                   "不可為小數、文字或符號。\n\n", token);
            continue;
        }

        /* (4) 負數 */
        if (value < 0) {
            printf("[錯誤] 不可輸入負數！費氏數列從第 0 項 F(0)=0 開始，"
                   "項數必須 >= 0。\n\n");
            continue;
        }

        /* (5) 超出可計算範圍 */
        if (value > MAX_N) {
            printf("[錯誤] 數字過大！F(%d) 以後會超出 long long 範圍造成溢位，"
                   "請輸入 0 ~ %d 之間的整數。\n\n", MAX_N + 1, MAX_N);
            continue;
        }

        return (int)value;   /* 全部檢查通過 */
    }
}

/* 把 clock() 的差值換算成毫秒 */
static double elapsed_ms(clock_t start, clock_t end) {
    return (double)(end - start) * 1000.0 / CLOCKS_PER_SEC;
}


int main() {
    /*
    TODO:
    1.顯示一段提示文字讓使用者分別輸入兩組數字
    2.對輸入的數字進行合法性驗證, 且若違反規則, 請對每一種違反規則的情況設計文字報錯, 並要求使用者重新輸入一組數字。
    註:Fibonacci(0) = 0 即為第零項，輸入只會有不會導致整數溢位的數字，但請思考費波納契數列在數學上的定義要求
    */
    int n, m;          /* 使用者輸入的起始項 n 與結束項 m */
    clock_t start, end;

    printf("========================================\n");
    printf("     費氏數列 Fibonacci Number 計算\n");
    printf("========================================\n");
    printf("說明：F(0)=0, F(1)=1, F(k)=F(k-1)+F(k-2)\n");
    printf("請輸入兩個整數 n、m（0 <= n <= m <= %d），\n", MAX_N);
    printf("程式將以四種方法列出第 n ~ m 個費氏數。\n");
    printf("（提示：遞迴法在 m > 40 時會非常慢，請耐心等候）\n\n");

    /*
     * 分別讀入 n 與 m，每個數字都會經過完整的合法性驗證。
     * 另外還要檢查兩數之間的關係：
     *   (6) n 不可大於 m —— 範圍必須由小到大，否則沒有可列出的項，
     *       此時要求使用者重新輸入一組 (n, m)。
     */
    while (1) {
        n = read_valid_number("請輸入起始項 n：");
        m = read_valid_number("請輸入結束項 m：");

        if (n > m) {
            printf("[錯誤] 起始項 n (%d) 不可大於結束項 m (%d)，"
                   "請重新輸入一組數字。\n\n", n, m);
            continue;
        }
        break;
    }

    printf("\n=== 迭代法計算結果 ===\n");

    /*
    TODO:利用Fibonacci_Iterative() 顯示計算結果
    */
    start = clock();
    Fibonacci_Iterative(n, m);
    end = clock();
    printf("耗時 %.3f ms\n", elapsed_ms(start, end));

    printf("\n=== 遞迴法計算結果 ===\n");

    /*
    TODO:利用Fibonacci_Recursion() 顯示計算結果
    */
    recursion_calls = 0;      /* 計算前把呼叫次數歸零 */
    start = clock();
    Fibonacci_Recursion(n, m, n);
    end = clock();
    printf("耗時 %.3f ms，計算單項的遞迴函式共被呼叫 %llu 次\n",
           elapsed_ms(start, end), recursion_calls);

    printf("\n=== 陣列法計算結果 ===\n");

    /*
    TODO:利用Fibonacci_Array() 顯示計算結果
    */
    start = clock();
    Fibonacci_Array(n, m);
    end = clock();
    printf("耗時 %.3f ms\n", elapsed_ms(start, end));

    printf("\n=== 公式法計算結果 ===\n");

    /*
    TODO:利用Fibonacci_Formula() 顯示計算結果
    */
    start = clock();
    Fibonacci_Formula(n, m);
    end = clock();
    printf("耗時 %.3f ms\n", elapsed_ms(start, end));

    return 0;
}
