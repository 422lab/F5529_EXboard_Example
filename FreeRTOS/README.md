# 使用注意事项

[当前 FreeRTOS 版本](https://github.com/FreeRTOS/FreeRTOS-Kernel/tree/49cb8e8b28c07db5b7739335f87829659e7beed7)

应当修正 FreeRTOSConfig 中的以下项  

configCPU_CLOCK_HZ 为 MCLK 时钟频率  
configTICK_RATE_HZ 为滴答时钟频率  

提示:  
如果使用驱动库, 可以使用如下代码获取当前频率  
```
// 告诉驱动库当前的两个外部晶振频率
UCS_setExternalClockSource(32768, 4000000);
// 由其计算时钟频率
uint32_t MCLK = UCS_getMCLK();
uint32_t SMCLK = UCS_getSMCLK();
uint32_t ACLK = UCS_getACLK();
```

通过 port.c 配置滴答时钟所使用的时钟(中断/时钟源/分频数)  
当前使用 WDT_A 时钟源 SMCLK 分频数 32K  
不改时钟的话, 只有 SMCLK / 分频数 = REFOCLK * 32倍频 / 32k分频 = 32768 * 32 / 32k = 32.768Hz 的滴答频率  
过高的滴答频率导致频繁的调度, 官方推荐 1000 我觉得对于430 这种玩意来说还是太高了  
当你使用  
4MHz 时 32k 分频是 125Hz  
8MHz 时 32k 分频是 250Hz  
16MHz 时 32k 分频是 500Hz  
所以对于要配置更高频率时 32k 分频是个不错的选择  


简单使用:
```
TaskHandle_t StartTask_Handler;

void start_task(void *pvParameters) {
    // 主体逻辑
}

int main(void) {
    WDTCTL = WDTPW | WDTHOLD;
    // 初始化代码
    xTaskCreate(start_task, "start_task", 32, NULL, 1, &StartTask_Handler);
    vTaskStartScheduler();
}
```

# 移植概要
将 FreeRTOS-Kernel 库根目录的源代码复制到 FreeRTOS  
将 FreeRTOS-Kernel/include 的头文件复制到 FreeRTOS  
将 FreeRTOS-Kernel/portable/CCS/MSP430X 下的文件复制到 FreeRTOS  
编写 vPortSetupTimerInterrupt 注册滴答定时器中断  
编写 vPortEndScheduler 暂停滴答定时器中断  
复制 heap_4.c 作为 FreeRTOS 栈内存分频手段  
复制 FreeRTOSConfig.h 并配置部分内容  
