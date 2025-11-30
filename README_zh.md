# esp32-signal-generator
基于ESP32微控制器家族构建的开源信号发生器。目前支持正弦波和方波。信号发生器的配置通过基于Web的用户界面完成，该界面直接通过WiFi从设备提供服务。

## 先决条件

### 硬件

开发和测试使用了[ESP32 DevKitC V4](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/hw-reference/modules-and-boards.html#esp32-devkitc-v4)。

### 软件

使用[Arduino](https://www.arduino.cc/en/Main/Software)软件版本1.8.15和[esp32板扩展](https://github.com/espressif/arduino-esp32)版本1.0.6进行构建和上传。要将静态Web文件上传到设备，请使用[ESP32文件系统上传器](https://randomnerdtutorials.com/install-esp32-filesystem-uploader-arduino-ide/)。

## 构建

使用Arduino进行构建和上传。
由于网站的静态文件是从简单的设备文件系统提供服务的，因此在首次使用前需要格式化闪存存储一次：

1. 在*SigGen.ino*中取消注释`#define FORMAT_FILESYSTEM`
2. 构建并上传
3. 在*SigGen.ino*中注释`#define FORMAT_FILESYSTEM`
4. 再次构建并上传
5. [可选] 如果更改了前端代码，请运行parcel
6. 在Arduino中：**工具 > ESP32草图数据上传**（这会将*/data*的内容复制到微控制器的闪存中）

## 首次运行

该软件配置为在启动后连接到WiFi网络，配置存储在微控制器的EEPROM中。在首次启动时，该配置未初始化，因此连接WiFi将失败。

### AP模式

每当无法建立WiFi连接时，设备将进入接入点（AP）模式，作为SSID为`SigGen`的WiFi接入点。可以通过在*SigGen.ino*中取消注释`#define FORCE_AP`来强制AP模式。

### WiFi配置

在AP模式下，设备的IP地址是`192.168.4.1`。要为WiFi配置设备，请导航到`http://<DEVICE_IP>`，在标题菜单中选择`配置`并设置SSID并输入密码。您可以选择为设备分配静态IP地址，以及配置网关和子网掩码。

![\"信号发生器的用户界面\"](/images/Screenshot_config.png)

将配置写入EEPROM后，您需要重新启动设备以应用更改。这可以通过设备本身的复位按钮或导航到`http://<DEVICE_IP>/reboot`来实现。

## 使用

访问`http://<DEVICE_IP>`以调出信号发生器的用户界面，该界面分为两个标签页（每个波形一个）。

### 方波

* **通道** [0-7]: 选择PWM通道。
* **定时器** [0-3]: 选择PWM定时器（每个定时器可以独立配置，并且多个通道可以连接到每个定时器）。
* **输出GPIO引脚** [PIN#]: 选择用于输出选定通道配置信号的GPIO引脚。
* **分辨率** [1-20]位: 设置选定定时器的分辨率（以位为单位）。
* **高速** [真/假]: 定义选定的定时器是否在高速模式下运行。
* **频率** [0-40M]Hz: 设置选定定时器的频率（以Hz为单位）。最大频率取决于所选的分辨率。
* **占空比** [0-100]%: 设置占空比（以%为单位）。
* **相位** [0-360]°: 设置相位偏移（以°为单位）。

![\"信号发生器的用户界面\"](/images/Screenshot_main_squarewave.png)

### 正弦波

* **通道** [1/2]: 选择DAC通道。ESP32有两个DAC通道（1和2），分别连接到GPIO引脚25和26。
* **时钟分频器** [0-7]: 从DAC时钟中为选定通道选择分频器，DAC时钟运行在135Hz。
* **反相** [关/全部/MSB/除MSB外全部]: 设置选定通道的反相模式（非反相正弦波使用MSB）。
* **频率** [17-135k]Hz: 设置选定通道的频率（以Hz为单位）。最小和最大频率取决于所选的时钟分频器（最小值 = 135/(clk_div + 1); 最大值 ≈ 135k/(clk_div + 1)）。DAC仍可在最大频率以上产生信号，但会逐渐失真。
* **相位** [0-360]°: 设置相位偏移（以°为单位）。这会切断正弦波的顶部。
* **幅度** [⅛/¼/½/1]: 将幅度设置为最大值（5V）的分数。

## 技术细节

### 前端

前端使用[Materialize](https://materializecss.com/)构建。由于Web服务器的限制，静态文件的数量已减少到实用最小值。因此，所有图像数据都内联定义，只有CSS文件与HTML分开提供。

### 工具

使用[Visual Studio Code](https://code.visualstudio.com/)版本1.43.1和[Arduino扩展](https://github.com/Microsoft/vscode-arduino.git)进行开发。

## Web API

文档待定（请参考UI的HTTP请求）。

## 未来工作

* 添加更多波形（三角波、锯齿波、任意波形）
* 改进WiFi连接性（处理断开连接）
* 蓝牙连接性

## 合作

如果您发现错误、有建议或有兴趣扩展此项目，请给我留言或提交拉取请求。