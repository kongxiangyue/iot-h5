/*---------------------------------------------------------------- 
  操作：
       1、设置模式  “自动获取” 或者 “手动获取”
       2、若 "手动获取" ， 点击“ 获取”
	   3、若 "自动获取" ， 点击“设置间隔” 进行计时获取温度  
-----------------------------------------------------------------*/
var timer1;
var num=0;
var clear_flag=1;
var change=0;
var js=0;
//---------------------------------------------------------------
function onConnect(){//网络连接成功将自动调用该函数，用户只需编写执行代码即可;该函数用来表现网络连接成功的状态
	/***************	****************/
	console.log("网络连接成功！");//向控制台输出内容
	/*******************************/
	
}
//---------------------------------------------------------------
function Disconnect(num){//网络连接断开将自动调用该函数，用户只需编写执行代码即可；该函数用来表现网络连接断开的状态
	if(num==1){//网络连接断开
		SocketConnect(2);//从新连接网络
	}
	else if(num==2){//网络连接异常
		alert("网络连接异常！");//网页弹出对话框显示文本"网络连接异常！"
	}
	
}
//---------------------------------------------------------------
function Data_Dispose(Data){//数据处理函数
	//console.log(Data);//控制台输出数据；控制台打开放式：用浏览器打开网页（.html文件），按键盘按键F12即可进入浏览器的调试台，选择“控制台”或“Console”选项；
	
}
//---------------------------------------------------------------
function TimeData(){   //获取数据的函数
				var Data=new Date();
				var Nian=Data.getFullYear()+"";
				var Yue=Data.getMonth()+1+"";
				var Ri=Data.getDate()+"";
				var Zhou=Data.getDay();
				var Shi=Data.getHours()+"";
				var Fen=Data.getMinutes()+"";
				var Miao=Data.getSeconds()+"";
				var ZhouStr="周";
				switch(Zhou){
					case 0:{ZhouStr="日";break;}
					case 1:{ZhouStr="一";break;}
					case 2:{ZhouStr="二";break;}
					case 3:{ZhouStr="三";break;}
					case 4:{ZhouStr="四";break;}
					case 5:{ZhouStr="五";break;}
					case 6:{ZhouStr="六";break;}
				}
				if(Yue.length==1) Yue="0"+Yue; 
				if(Ri.length==1) Ri="0"+Ri; 
				if(Shi.length==1) Shi="0"+Shi; 
				if(Fen.length==1) Fen="0"+Fen; 
				if(Miao.length==1) Miao="0"+Miao; 
				var str="当前时间： "+Nian+" 年 "+Yue+" 月 "+Ri+" 日 "+ZhouStr+" "+Shi+":"+Fen+":"+Miao;
				document.getElementById("TimeText").innerHTML=str+"";
}

function TXD()
{
	Send_Data_Str("TEMP=?");  //发送数据
	//console.log(dat);
}

function Time_Interval()  //计时
{
	if(clear_flag==1)
	{    
		if(timer1!=null)clearInterval(timer1);  //清空定时器
	}
	else{
		num=document.getElementById("text1").value; //获取文本框内的值
		if(num!=null&&num!=0)
		{
			timer1=setInterval(TXD,num*1000);  //定时num秒发送数据	
		}
	}
}

function  Get_Set()   //获取温度 或者 设置时间
{
	if(js==1)    //计时标志位 （自动获取时,js=1）
	{
		clear_flag=0;   //不清空计时
		Time_Interval(); //开始计时
		
	}
	else if(js==0)  //计时标志位 
	{
		TXD();	//发送数据，获取温度
		
	} 
	
}		
/*-------------------------------------------------------------------
 注：  界面默认显示：手动获取            默认： change=0  js=0;
      
    1、按键被按下，显示“自动获取”,change=1,js=1; 说明开始自动获取温度,开始计时。
	   
	2、按键再被按下，显示"手动获取",change=0,js=0; 说明需手动获取温度,关计时。
	  
--------------------------------------------------------------------*/
function GetTemp_Mode()//获取温度的模式
{
	if(change==0)  //change默认为0,即按键被按下,change=1,显示“自动获取”
	{
		document.getElementById("But").innerHTML="自动获取";  
		document.getElementById("But_1").innerHTML="设置间隔";
		document.getElementById("text1").disabled="";
		change=1;  //模式切换标志位
		js=1;      //计时标志位
	}
	else if(change==1) //按键被按下,change=0,显示"手动获取",计时
	{
		//当显示"手动获取"时,上位机处于"自动获取"状态
		document.getElementById("But").innerHTML="手动获取";
		document.getElementById("But_1").innerHTML="获取";
		document.getElementById("text1").disabled="disabled";
		document.getElementById("text1").value="";
		clear_flag=1;     //清空计数器标志位
		Time_Interval();  //计时
		change=0;         //模式切换标志位
		js=0;
	}
}