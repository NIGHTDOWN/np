#include <shellapi.h>  
  
int main() {  
  char* url = "http://www.example.com";  
      
    char * result = ShellExecute(NULL, "open", url, NULL, NULL, SW_SHOWNORMAL);  
    if ((int)result <= 32) {  
        // 处理错误或弹出错误消息  
    }  
  
    return 0;  
}