// JavaScript Document
var LedStatu1="OFF";
var LedStatu2="OFF";
var LedStatu3="OFF";
var Networking=0;
var LightOFF="Light=0\r\n";
var LightON="Light=1\r\n";
var TVOFF="TV=0\r\n";
var TVON="TV=1\r\n";
var curtainsOFF="curtains=0\r\n";
var curtainsON="curtains=1\r\n";
var LockON="Lock=1\r\n";
var Read_tempr="Take\r\n";
var Tem=0;
//------------------------------------------------------------------------
function StartLoad()                                      //网页联网
{
	SocketConnect(2);   
	//websocket1.send("state=?");                               //连接端口1
}
//-----------------------------------------------------------------------
function onConnect()                                   //联网成功
{
	Networking=1;                                      //联网成功标志
	//window.alert("12345");
	WebSocket_Send(2,"state=?\r\n");
}


//-----------------------------------------------------------------------
function Connecting()                                  //断网
{
	SocketConnect(2);
}
function Read_Temperature()
{
	if(Networking!=1)
	{
		return;
	}
	WebSocket_Send(2,"Take\r\n");
}
//--------------------------------------------------------------------
function deng()
{
	if(Networking!=1)
	{
		return;
	}
	if(LedStatu1=="OFF")
	{
		WebSocket_Send(2,LightOFF);
		LedStatu1="ON";
	}
	else
	{
		WebSocket_Send(2,LightON);
		LedStatu1="OFF";	
	}
}
//---------------------------------------------------------------	
function TV()
{
	if(Networking!=1)
	{
		return;
		console.log(12345);
	}
	if(LedStatu2=="OFF")
	{
		WebSocket_Send(2,TVOFF);
		LedStatu2="ON";
		console.log(TVOFF);
	}
	else
	{
		WebSocket_Send(2,TVON);
		LedStatu2="OFF";
		console.log(TVON);
	}
}
//------------------------------------------------------------------
function chuanl()
{
	if(Networking!=1)
	{
		return;
	}
	if(LedStatu3=="OFF")
	{
		WebSocket_Send(2,curtainsOFF);
		LedStatu3="ON";
	}
	else
	{
		WebSocket_Send(2,curtainsON);
		LedStatu3="OFF"
	}
}
function door()
{
	if(Networking!=1)
	{
		return;
	}
	self.location="doos.html";
}
//-----------------------------------------------------------------------------
function TimeData()
{
	if(Networking!=1)
	{
		return;
	}
	else 
	{
		var Data=new Date();                               //获取数据格式
		var Nian=Data.getFullYear()+"";                    //获取哪年份
		var Yue=Data.getMonth()+1+"";                      //获取哪月份
		var Ri=Data.getDate()+"";                          //获取哪日
		var Zhou=Data.getDay();                            //获取哪周
		var Shi=Data.getHours()+"";                        //获取小时 
		var Fen=Data.getMinutes()+"";                      //获取分钟
		var Miao=Data.getSeconds()+"";                     //获取秒钟
		var ZhouStr="";                                    //先显示周字
		switch(Zhou)
		{
			case 0:{ZhouStr="周日";break;}
			case 1:{ZhouStr="周一";break;}
			case 2:{ZhouStr="周二";break;}
			case 3:{ZhouStr="周三";break;}
			case 4:{ZhouStr="周四";break;}
			case 5:{ZhouStr="周五";break;}
			case 6:{ZhouStr="周六";break;}
		}
		if(Yue.length==1) Yue="0"+Yue; 
		if(Ri.length==1)  Ri="0"+Ri; 
		if(Shi.length==1) Shi="0"+Shi; 
		if(Fen.length==1) Fen="0"+Fen; 
		if(Miao.length==1)Miao="0"+Miao; 
		var str1=""+Nian+" 年 "+Yue+" 月 "+Ri+" 日"+ZhouStr+" ";
		var str2=""+Shi+":"+Fen+":"+Miao;
		document.getElementById("date").innerHTML=str1+"";	
		document.getElementById("time").innerHTML=str2+"";
	}
}
//---------------------------------------------------------------
function onReceive(nSocket,str)
{
   if(nSocket!=2)
   {
	   return;
   }
   if(str.indexOf(LightOFF)!=-1)
   {
	   LedStatu1="ON";
	   document.getElementById("pic").src="img/deng.png";
	   document.getElementById("buts1").src="img/but1.png";
	   return;
   }
   if(str.indexOf(LightON)!=-1)
   {
	   LedStatu1="OFF";
	   document.getElementById("pic").src="img/dengs.png";
	   document.getElementById("buts1").src="img/but_1.png";
	   return;
   }
   if(str.indexOf(TVOFF)!=-1)
   {
	
	   LedStatu2="ON";
	   document.getElementById("tpic").src="img/TV_off.png";
	   document.getElementById("buts2").src="img/but2.png";
	   return;
   }
   if(str.indexOf(TVON)!=-1)
   {
	   LedStatu2="OFF";
	   document.getElementById("tpic").src="img/TV_on.png";
	   document.getElementById("buts2").src="img/but_2.png"; 
	   return;  
   }
   if(str.indexOf(curtainsOFF)!=-1)
   {
	   LedStatu3="ON";
	   document.getElementById("ctic").src="img/chuangg.gif";
	   document.getElementById("buts3").src="img/but3.png";
	   return;
   }
   if(str.indexOf(curtainsON)!=-1)
   {
	   LedStatu3="OFF";
	   document.getElementById("ctic").src="img/chuangk.gif";
	   document.getElementById("buts3").src="img/but_3.png";
	   return;  
   }
   if(str.substr(0,2)=="T:")
   {
	   Tem=str.substr(2,4);
	   document.getElementById("tempr").innerHTML=Tem;
   }
}
//----------------------------------------------------------------


