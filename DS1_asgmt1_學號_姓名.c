#include <stdio.h>
/*
請依需求自由加入標頭檔
*/
#include <stdlib.h>   /* malloc / free / strtoll                         */
#include <string.h>   /* strlen / strtok / strchr                        */
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


/*
以下四個函式名稱請勿更動
型別、使用方式、參數列、回傳型別、輸出方式、所需輸入引數及變數請自行設計
*/

/* ====================================================================
 * 1. 迭代法 (Iterative)
 * --------------------------------------------------------------------
 * 參數：n  要計算的項數 (n >= 0)
 * 回傳：F(n)
 *
 * 原理：
 *   F(0) = 0、F(1) = 1、F(n) = F(n-1) + F(n-2)
 *   只要記住「前兩項」，用迴圈一路往後推，就能得到第 n 項。
 *
 * 效率：
 *   時間複雜度 O(n)  —— 迴圈只跑 n-1 次
 *   空間複雜度 O(1)  —— 只用到 prev、curr、next 三個變數
 * ==================================================================== */
long long Fibonacci_Iterative(int n) {
    long long prev = 0;   /* 代表 F(i-1)，一開始是 F(0) = 0 */
    long long curr = 1;   /* 代表 F(i)  ，一開始是 F(1) = 1 */
    long long next;       /* 暫存 F(i+1)                    */
    int i;

    /* 邊界條件：第 0 項與第 1 項直接回傳定義值 */
    if (n == 0) return 0;
    if (n == 1) return 1;

    /* 從第 2 項算到第 n 項，每次把「前兩項」往後挪一格 */
    for (i = 2; i <= n; i++) {
        next = prev + curr;   /* F(i) = F(i-1) + F(i-2) */
        prev = curr;          /* 舊的 F(i-1) 變成新的 F(i-2) */
        curr = next;          /* 新算出的值變成新的 F(i-1)   */
    }
    return curr;              /* 迴圈結束時 curr 就是 F(n) */
}

/* ====================================================================
 * 2. 遞迴法 (Recursion)
 * --------------------------------------------------------------------
 * 參數：n  要計算的項數 (n >= 0)
 * 回傳：F(n)
 *
 * 原理：
 *   直接把數學定義翻譯成程式：
 *     F(n) = F(n-1) + F(n-2)，終止條件 F(0)=0、F(1)=1
 *
 * 效率：
 *   時間複雜度 O(2^n)（更精確約 O(1.618^n)）
 *   —— 因為同一個子問題會被重複計算非常多次，
 *      例如算 F(5) 時，F(3) 被算 2 次、F(2) 被算 3 次……
 *   空間複雜度 O(n)  —— 遞迴呼叫堆疊的最大深度為 n
 *   n 大約超過 40 之後，執行時間會明顯變長。
 * ==================================================================== */
long long Fibonacci_Recursion(int n) {
    recursion_calls++;        /* 每進入一次函式就計數一次 */

    /* 終止條件 (base case)：沒有它，遞迴會無限呼叫下去 */
    if (n == 0) return 0;
    if (n == 1) return 1;

    /* 遞迴關係：把大問題拆成兩個較小的子問題 */
    return Fibonacci_Recursion(n - 1) + Fibonacci_Recursion(n - 2);
}

/* ====================================================================
 * 3. 陣列法 (Array / 動態規劃 Dynamic Programming)
 * --------------------------------------------------------------------
 * 參數：n  要計算的項數 (n >= 0)
 * 回傳：F(n)；若記憶體配置失敗則回傳 -1
 *
 * 原理：
 *   建一個長度 n+1 的陣列 fib[]，fib[i] 存放 F(i)。
 *   先填入 fib[0]=0、fib[1]=1，再由小到大依序
 *   fib[i] = fib[i-1] + fib[i-2]，每一項只算一次，
 *   之後要用時直接查表，避免遞迴法的重複計算。
 *
 * 效率：
 *   時間複雜度 O(n)
 *   空間複雜度 O(n)  —— 需要一整個陣列存放所有項
 *   （與迭代法相比，多花了記憶體，但保留了所有中間結果）
 * ==================================================================== */
long long Fibonacci_Array(int n) {
    long long *fib;           /* 指向動態配置的陣列 */
    long long result;
    int i;

    /* 依 n 的大小動態配置陣列；至少配 2 格以便放 fib[0]、fib[1] */
    fib = (long long *)malloc(sizeof(long long) * (n < 2 ? 2 : n + 1));
    if (fib == NULL) {
        printf("記憶體配置失敗！\n");
        return -1;
    }

    /* 初始值（數學定義） */
    fib[0] = 0;
    fib[1] = 1;

    /* 由小到大填表：每一格都由前兩格相加而來 */
    for (i = 2; i <= n; i++) {
        fib[i] = fib[i - 1] + fib[i - 2];
    }

    result = fib[n];          /* 取出第 n 項 */
    free(fib);                /* 用完記得釋放記憶體，避免 memory leak */
    return result;
}

/* ====================================================================
 * 4. 公式法 (Binet's Formula 比內公式)
 * --------------------------------------------------------------------
 * 參數：n  要計算的項數 (n >= 0)
 * 回傳：F(n)
 *
 * 原理：
 *   費氏數列有封閉形式（一般項）公式：
 *
 *            1    [ (1 + √5)^n     (1 - √5)^n ]
 *   F(n) = ---- × [ (-------)   -  (-------)  ]
 *           √5    [ (   2   )      (   2   )  ]
 *
 *   其中 φ = (1+√5)/2 ≈ 1.618 為黃金比例，ψ = (1-√5)/2 ≈ -0.618。
 *
 * 效率：
 *   理論上時間複雜度 O(1)（或 O(log n)，取決於 pow 的實作），
 *   不需要任何迴圈或遞迴，速度最快。
 *
 * 注意（浮點數精度問題）：
 *   √5 是無理數，電腦只能以近似值儲存，n 越大誤差越大。
 *   這裡使用 long double 並以 roundl() 四捨五入到最近整數來降低誤差；
 *   double 大約只能正確算到 F(70) 左右，
 *   long double 的精度則依編譯器而定（gcc/x86 較高，MSVC 等同 double）。
 *   因此 main() 中會將公式法結果與迭代法比對，若不一致會標示出來。
 * ==================================================================== */
long long Fibonacci_Formula(int n) {
    long double sqrt5 = sqrtl(5.0L);              /* √5           */
    long double phi   = (1.0L + sqrt5) / 2.0L;    /* φ 黃金比例   */
    long double psi   = (1.0L - sqrt5) / 2.0L;    /* ψ 共軛根     */

    /* 代入比內公式，並四捨五入消除浮點誤差 */
    long double value = (powl(phi, n) - powl(psi, n)) / sqrt5;
    return (long long)roundl(value);
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

    if (end == str)  return 0;        /* 完全沒有讀到數字，如 "abc"     */
    if (*end != '\0') return 0;       /* 數字後面還有其他字元，如 "3.5" */
    if (errno == ERANGE) {            /* 數字大到 long long 都放不下   */
        value = (value > 0) ? MAX_N + 1 : -1;   /* 交給後續的範圍檢查處理 */
    }
    *out = value;
    return 1;
}

/* ====================================================================
 * 輔助函式：讀取並驗證使用者輸入的「一組數字」
 * --------------------------------------------------------------------
 * 參數：prompt  顯示給使用者的提示文字
 * 回傳：通過所有驗證的項數 n (0 <= n <= MAX_N)
 *
 * 會針對每一種違規情況顯示不同的錯誤訊息，並要求重新輸入，
 * 直到使用者輸入合法數字為止：
 *   (1) 沒有輸入任何內容（空白行）
 *   (2) 一次輸入了多個數字
 *   (3) 輸入的不是整數（小數、英文字母、特殊符號……）
 *   (4) 輸入負數 —— 費氏數列的項數在數學上定義於非負整數，
 *                   F(0) 為第零項，不存在「第 -1 項」
 *   (5) 輸入超過 MAX_N —— 結果會超出 long long 範圍而溢位
 * ==================================================================== */
static int read_valid_n(const char *prompt) {
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
    int nums[2];       /* 使用者輸入的兩組數字 */
    int k;
    long long result;
    clock_t start, end;

    printf("========================================\n");
    printf("     費氏數列 Fibonacci Number 計算\n");
    printf("========================================\n");
    printf("說明：F(0)=0, F(1)=1, F(n)=F(n-1)+F(n-2)\n");
    printf("請分別輸入兩組數字 n（0 ~ %d 的整數），\n", MAX_N);
    printf("程式將以四種方法計算第 n 項費氏數 F(n)。\n");
    printf("（提示：遞迴法在 n > 40 時會非常慢，請耐心等候）\n\n");

    /* 分別讀入兩組數字，每一組都會經過完整的合法性驗證 */
    nums[0] = read_valid_n("請輸入第一組數字 n1：");
    nums[1] = read_valid_n("請輸入第二組數字 n2：");

    printf("\n=== 迭代法計算結果 ===\n");

    /*
    TODO:利用Fibonacci_Iterative() 顯示計算結果
    */
    for (k = 0; k < 2; k++) {
        start  = clock();
        result = Fibonacci_Iterative(nums[k]);
        end    = clock();
        printf("F(%d) = %lld\t（耗時 %.3f ms）\n",
               nums[k], result, elapsed_ms(start, end));
    }

    printf("\n=== 遞迴法計算結果 ===\n");

    /*
    TODO:利用Fibonacci_Recursion() 顯示計算結果
    */
    for (k = 0; k < 2; k++) {
        recursion_calls = 0;              /* 每次計算前歸零呼叫次數 */
        start  = clock();
        result = Fibonacci_Recursion(nums[k]);
        end    = clock();
        printf("F(%d) = %lld\t（耗時 %.3f ms，函式共被呼叫 %llu 次）\n",
               nums[k], result, elapsed_ms(start, end), recursion_calls);
    }

    printf("\n=== 陣列法計算結果 ===\n");

    /*
    TODO:利用Fibonacci_Array() 顯示計算結果
    */
    for (k = 0; k < 2; k++) {
        start  = clock();
        result = Fibonacci_Array(nums[k]);
        end    = clock();
        printf("F(%d) = %lld\t（耗時 %.3f ms）\n",
               nums[k], result, elapsed_ms(start, end));
    }

    printf("\n=== 公式法計算結果 ===\n");

    /*
    TODO:利用Fibonacci_Formula() 顯示計算結果
    */
    for (k = 0; k < 2; k++) {
        start  = clock();
        result = Fibonacci_Formula(nums[k]);
        end    = clock();
        printf("F(%d) = %lld\t（耗時 %.3f ms）", nums[k], result, elapsed_ms(start, end));

        /* 與迭代法（精確整數運算）比對，展示浮點數精度的限制 */
        if (result != Fibonacci_Iterative(nums[k])) {
            printf("  ※ 與正確值 %lld 不符：浮點數精度不足造成誤差",
                   Fibonacci_Iterative(nums[k]));
        }
        printf("\n");
    }

    return 0;
}
