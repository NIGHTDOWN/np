#include <windows.h>  
#include <stdio.h>  
#include <shellapi.h>
  //编译
  //-mwindows表示无控制台窗口
//   windres hyt.rc -O coff -o hyt.res   生成图标资源文件
//   gcc -o myprogram.exe hyt.c hyt.res -mwindows      生成应用程序
int main() {  
    
    // // 打开指定的URL  
    // ShellExecute(NULL, "open", "https://www.huyintong.com/", NULL, NULL, SW_SHOWNORMAL);  
  
    // return 0;  
    //不显示浏览器控件
    const char* url = "https://www.huyintong.com/";  
    const char* operation = "open";  
    SHELLEXECUTEINFO sei = { sizeof(sei) };  
    sei.hwnd = NULL;  
    sei.lpFile = url;  
    sei.lpParameters = operation;  
    sei.nShow = SW_HIDE;  
    sei.fMask = SEE_MASK_INVOKEIDLIST;  
    ShellExecuteEx(&sei);  
  
    return 0;  
}


// #include <stdio.h>  
// #include <stdlib.h>  
// #include <string.h>  
// #include <curl/curl.h>  
// #include <WebKit/WebKit.h>  
  
// int main() {  
//     // 初始化libcurl库  
//     curl_global_init(CURL_GLOBAL_DEFAULT);  
  
//     // 创建Webview控件  
//     HWND webViewWindow = WebView_Create(NULL, 800, 600);  
  
//     // 加载百度页面  
//     WebView_LoadURL(webViewWindow, "http://www.baidu.com");  
  
//     // 显示Webview控件  
//     WebView_Show(webViewWindow, WebViewShowWindowCommandVisible);  
  
//     // 等待页面加载完成  
//     while (WebView_WaitForDoneLoading(webViewWindow) ==WebViewDoneLoadingStateOngoing) {  
//         // 等待一段时间，然后继续等待  
//         Sleep(100);  
//     }  
  
//     // 销毁Webview控件  
//     WebView_Destroy(webViewWindow);  
  
//     // 清理libcurl库  
//     curl_global_cleanup();  
  
//     return 0;  
// }