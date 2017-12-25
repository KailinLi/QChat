# QChat

目前本机传输速度约为50M/s, 两台电脑局域网传输约为11M/s

基于QTcpSocket和QUdpSocket实现的即时聊天工具

* 工具包括服务器端和客户端;
* 具备用户注册、登录、找回密码功能(基于TCP协议);
* 两个用户如果同时在线，采用点到点通信方式进行聊天，信息不需要通过服务器中转，服务器也不保存(基于 TCP 协议);
* 支持离线消息(基于TCP协议);
* 支持点到点可靠文件传输(基于UDP协议);
* 存储在服务器端的数据需要进行强加密;
* 支持多组用户同时在线交流和传输文件;
* 文件传输具有良好的性能，能够充分利用网路带宽;
* 人机交互友好，软件易用性强。

## Demo

![](http://oklhb00qa.bkt.clouddn.com/image001.png)

![](http://oklhb00qa.bkt.clouddn.com/image041.png)

![](http://oklhb00qa.bkt.clouddn.com/image043.png)

![](http://oklhb00qa.bkt.clouddn.com/image112.png)