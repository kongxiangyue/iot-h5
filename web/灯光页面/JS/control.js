// JavaScript Document
function Connect()  
{
	if(websocket_Connected==1)
	{
		window.alert("联网成功");	
		Send_Data_Str("Light=?");
	}
}

function Send (Server_Data)   //数据发送
{
	Send_Data_Str(Server_Data);
}


function Turn_On()   //开界面灯
{
    var x=document.getElementById("dgm");
	x.setAttribute("src","ing/timg2.png");
}

function Turn_Off() //关界面灯
{
    //语法:element.setAttribute(attributename,attributevalue)  
    // 参数:attributename	类型:String   描述:必需填写。您希望添加的属性的名称。
    // 参数:attributevalue	类型:String   描述:必需填写。您希望添加的属性值。
	var y=document.getElementById("dgm");  
	y.setAttribute("src","ing/timg.png");	
}

function Button_On()    //点击界面的"开",触发button_on()
{
	Send("Light=0");	
}

function Button_Off()  //点击界面的"关",触发button_off()
{
	Send("Light=1");	
}

// 这个函数名已经在websocket.js里写死了
// 换句话来说,Server_Receive是作为固定的接收数据的函数
function Server_Receive(str)  //数据接收处理
{
	var R_data=str;
	if( R_data=="Light=0\r\n" )
	{
	      Turn_On();
	}
	if(R_data=="Light=1\r\n")
	{
	       Turn_Off();
	}
	
}

