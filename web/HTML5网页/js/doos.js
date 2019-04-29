// JavaScript Document
var PassWordStr="";
var Networking=0;
//=================================================================
function StartLoad()                                      //网页联网
{
	SocketConnect(2);                                  //连接端口1
	PassWordStr="";
}
//-----------------------------------------------------------------------
function onConnect()                                   //联网成功
{
	Networking=1;                                      //联网成功标志
	WebSocket_Send(2,"state=?\r\n");
}
//-----------------------------------------------------------------------
function Connecting()                                  //断网
{
	SocketConnect(2);
}
//---------------------------------------------------------------
function Key0()
{
   PassWordStr=PassWordStr+"0";
   document.getElementById("PassWord").innerHTML=PassWordStr; 
}
//---------------------------------------------------------------
function Key1()
{
   PassWordStr=PassWordStr+"1";
   document.getElementById("PassWord").innerHTML=PassWordStr; 
}
//---------------------------------------------------------------
function Key2()
{
   PassWordStr=PassWordStr+"2";
   document.getElementById("PassWord").innerHTML=PassWordStr; 
}
//---------------------------------------------------------------
function Key3()
{
   PassWordStr=PassWordStr+"3";
   document.getElementById("PassWord").innerHTML=PassWordStr; 
}
//---------------------------------------------------------------
function Key4()
{
   PassWordStr=PassWordStr+"4";
   document.getElementById("PassWord").innerHTML=PassWordStr; 
}
//---------------------------------------------------------------
function Key5()
{
   PassWordStr=PassWordStr+"5";
   document.getElementById("PassWord").innerHTML=PassWordStr; 
}
//---------------------------------------------------------------
function Key6()
{
   PassWordStr=PassWordStr+"6";
   document.getElementById("PassWord").innerHTML=PassWordStr; 
}
//---------------------------------------------------------------
function Key7()
{
   PassWordStr=PassWordStr+"7";
   document.getElementById("PassWord").innerHTML=PassWordStr; 
}
//---------------------------------------------------------------
function Key8()
{
   PassWordStr=PassWordStr+"8";
   document.getElementById("PassWord").innerHTML=PassWordStr; 
}
//---------------------------------------------------------------
function Key9()
{
   PassWordStr=PassWordStr+"9";
   document.getElementById("PassWord").innerHTML=PassWordStr; 
}
//---------------------------------------------------------------
function Key10()
{
   PassWordStr=PassWordStr+"*";
   document.getElementById("PassWord").innerHTML=PassWordStr; 
}
//---------------------------------------------------------------
function Key11()
{
   PassWordStr=PassWordStr+"#";
   document.getElementById("PassWord").innerHTML=PassWordStr; 
}
//---------------------------------------------------------------
function KeyCLR()
{
   PassWordStr="";
   document.getElementById("PassWord").innerHTML=PassWordStr; 
}
//---------------------------------------------------------------
function KeyF1()
{
   var str= "Light=1";
   if(websocket2_Connected==1)
   {	
      WebSocket_Send(2,str);
   }
   else
   {
	  window.alert("提示：网络没有连接......");
   }   
}
//---------------------------------------------------------------
function Return()
{
   	if(Networking!=1)
	{
		return;
	}
	self.location="index.html";
}
//---------------------------------------------------------------
function KeyOK()
{
   var str=':'+PassWordStr+"\r\n";
   if(websocket2_Connected==1)
   {	
      WebSocket_Send(2,str);
	  console.log(str);
   }
   else
   {
	  window.alert("提示：网络没有连接......");
   }  
}
//---------------------------------------------------------------
function onReceive(nSocket,str)
{
   if(nSocket!=2)
   {
	   return;
   }
   if(str=="open the door\r\n")
   {
	
	   PassWordStr="开门";   
   	   document.getElementById("PassWord").innerHTML=PassWordStr;
	   document.getElementById("pic").src="img/door1.png";
	   
   }
   if(str=="wrong password\r\n")
   {
	   PassWordStr="密码错误";  
	   document.getElementById("pic").src="img/door.png"; 
   	   document.getElementById("PassWord").innerHTML=PassWordStr;
   }
   if(str=="being revised\r\n")
   {
	   PassWordStr="修改密码中......";   
   	   document.getElementById("PassWord").innerHTML=PassWordStr;
   }
   if(str=="signal if\r\n")
   {
	   PassWordStr="修改密码成功";   
   	   document.getElementById("PassWord").innerHTML=PassWordStr;
   }
   if(str=="Input error\r\n")
   {
	   PassWordStr="修改密码格式错误";   
   	   document.getElementById("PassWord").innerHTML=PassWordStr;
   }
   if(str=="in recovery\r\n")
   {
	   PassWordStr="恢复默认密码中......";   
   	   document.getElementById("PassWord").innerHTML=PassWordStr;
   }
   if(str=="Restore success\r\n")
   {
	   PassWordStr="恢复默认密码成功";   
   	   document.getElementById("PassWord").innerHTML=PassWordStr;
   }
   if(str=="empty\r\n")
   {
	   PassWordStr="";   
   	   document.getElementById("PassWord").innerHTML=PassWordStr;
	   document.getElementById("pic").src="img/door.png";
   }
}

//===============================================================