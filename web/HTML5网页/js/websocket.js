//=================================================================
//
//  程序名: websocket.js
//  说 明： webSocket 网络连接函数，一般不需要修改可直接调用
//  
//=================================================================

//定义全局变量
var websocket1_Connected,websocket2_Connected; //两个端口的网络连接标志位，0--断开，1--连接
var websocket1,websocket2;                   //两个端口连接的句柄
var Cam_Uri;                                 //视频链接地址
var Temp;
//-----------------------------------------------------------------
function SocketConnect(nSocket)   //websocket 连接函数，参数nSocket 1--连接端口1，2--连接端口2
{	
   var Uri1,Uri2;
   GetIP();
   var nPort1 = parseInt(FrunWeb_PORT)+1; //获取HTML5 Web单片机模块的端口1
   var nPort2 = parseInt(FrunWeb_PORT)+2; //获取HTML5 Web单片机模块的端口2
   var nPort3 = parseInt(FrunWeb_PORT)+3; //获取HTML5 Web单片机模块摄像机端口
   
   Cam_Uri="http://"+FrunWeb_IP+":"+nPort3.toString();
   
   if ("WebSocket" in window) 
   {
   } 
   else 
   {
      window.alert("提示:该浏览器不支持HTML5，建议选择Google，FireFox浏览器！");
	  return;
   }   
   Uri1="ws://"+FrunWeb_IP+":"+nPort1.toString();
   Uri2="ws://"+FrunWeb_IP+":"+nPort2.toString();    
   try 
   {
      if(nSocket==1)
	  {
         websocket1_Connected=0;                                              //1口网络断开
         websocket1 = new WebSocket(Uri1);                                    
		 websocket1.onopen    = function (evt) { websocket1_Open(evt)   };    //1口连接成功
         websocket1.onclose   = function (evt) { websocket1_Close(evt)  };    //1口关闭网络
         websocket1.onmessage = function (evt) { websocket1_Message(evt)};    //1口收到数据
         websocket1.onerror   = function (evt) { websocket1_Error(evt)  };	  //1口通讯错误	
	  }
	  else
	  {
		 websocket2_Connected=0;                                              //2口网络断开
		 websocket2 = new WebSocket(Uri2);  
		 websocket2.onopen    = function (evt) { websocket2_Open(evt)   };    //2口连接成功
         websocket2.onclose   = function (evt) { websocket2_Close(evt)  };    //2口关闭网络
         websocket2.onmessage = function (evt) { websocket2_Message(evt)};    //2口收到数据
         websocket2.onerror   = function (evt) { websocket2_Error(evt)  };    //2口通讯错误	
	  }
   } 
   catch (err){window.alert("提示：连接错误，请重新连接！");}       
}
//----------------------------------------------------------------
function websocket1_Open(evt)                                                 //1口连接成功
{
   websocket1_Connected=1;                                                   //标致制1
   onConnect(1);                                                              //函数自己做可以返回网页显示程序对不对
   
}
//---------------------------------------------------------------
function websocket1_Close(evt)                                                //1口关闭网咯
{
   websocket1_Connected=0;
   Connecting(1);
}
//---------------------------------------------------------------
function websocket1_Error(evt)                                                //1口通讯错误
{
   websocket1_Connected=0;
}
//---------------------------------------------------------------
function websocket2_Open(evt)                                                //2口连接成功
{
   websocket2_Connected=1;                                                   //标致制
   onConnect(2);                                                             //函数自己做可以返回网页显示程序对不对
}
//---------------------------------------------------------------
function websocket2_Close(evt)                                               //2口关闭网咯
{
   websocket2_Connected=0;
   Connecting(2);
}
//---------------------------------------------------------------
function websocket2_Error(evt)                                               //2口通讯错误
{
   websocket2_Connected=0;
}
//---------------------------------------------------------------
function websocket1_Message(evt)                                             //1口收到数据函数
{  
   var recstr=evt.data+"\r\n";                                               //定义字符串变量  recstr  为字符
    console.log(recstr);
   onReceive(1,recstr);
                                                         //收到什么数据。函数自己写。
}
//---------------------------------------------------------------
function websocket2_Message(evt)                                             //2口收到数据函
{	
   var recstr=evt.data;                                               //定义字符串变量  recstr  为
  
	var str;
    var reader = new FileReader();
	reader.readAsText(recstr,'utf-8'); 
	reader.onload = function(evt){
		str=reader.result;
		//此处根据实际需要编写相应的数据处理代码
	    onReceive(2,str);
	};		                                                      //收到什么数据。函数自己
}
//---------------------------------------------------------------
function WebSocket_Send(nSocket,str)                                         //发送数据
{  
   try 
   {
	 if(nSocket==1)
	 {  
        websocket1.send(str); 
	 }
	 else
	 {
		websocket2.send(str); 
	 }
   }
   catch (err){window.alert("提示：数据发送错误，请重新发送！");} 
}
//--------------------------------------------------------------------
function GetIP()                                                            //自动从框里获取IP
{
  /* var str,ip;
   str=window.location.href;
   str=str.split("/",10);
   ip=str[2].split(":",2);
   FrunWeb_IP=ip[0];
   FrunWeb_PORT=ip[1];*/
   FrunWeb_IP="192.168.1.254";
   FrunWeb_PORT="5000";  
}
//----------------------------------------------------------------/ JavaScript Document
//----------------------------------------------------------------/ JavaScript Document