using System;
using System.Collections.Generic;
using System.Net;
using System.Net.Sockets;

namespace IPC_MVR_Manager
{
	public class LogHelper
	{
		TcpListener logListener;
		TcpClient logClient;
		
		public LogHelper() {
			TcpListener tmp = new TcpListener(IPAddress.Any, 0);
			tmp.Start();
			logListener = new TcpListener(IPAddress.Any,((IPEndPoint) tmp.LocalEndpoint).Port);
			tmp.Stop();
			logListener.Stop();
		}
		public void Close() {
			if (logClient != null)
				logClient.Close();
			logClient = null;
			//logListener.Stop();
			if (LogDisconnectEvent != null) {
				LogDisconnectEvent();
			}
		}
		public void Open()
		{
	
			if (logClient != null)
				logClient.Close();
			logClient = null;
			logListener.Start();
			logListener.BeginAcceptTcpClient(doAcceptTcpClientCallback, null);
		}
		private void doAcceptTcpClientCallback(IAsyncResult ar)
		{
			try
			{
				logClient = logListener.EndAcceptTcpClient(ar);
			}
			catch
			{
				Close();
				return;
			}


			Dictionary<string, object> readParams = new Dictionary<string, object>();
			readParams["logClient"] = logClient;
			byte[] buffer = new byte[1024];
			readParams["buffer"] = buffer;
			logClient.GetStream().BeginRead(buffer, 0, 4, doRecvSize, readParams);
		}
		public delegate void LogDisconnect();
		public event LogDisconnect LogDisconnectEvent;
		private void doRecvSize(IAsyncResult ar)
		{
			Dictionary<string, object> readParams = ar.AsyncState as Dictionary<string, object>;
			TcpClient logClient = readParams["logClient"] as TcpClient;
			byte[] buffer = readParams["buffer"] as byte[];
			try
			{
				logClient.GetStream().EndRead(ar);
				UInt32 size = BitConverter.ToUInt32(buffer, 0);
				readParams["size"] = BitConverter.GetBytes(size);
				logClient.GetStream().BeginRead(buffer, 0, (int)size, doRecvLog, readParams);

			}
			catch
			{
				Close();
				//LogDisconnectEvent();
				//listBox.Dispatcher.Invoke(() => { logBtn.Content = "开启"; });

			}
		}
		public UInt16 LogPort {
			get {
				return (UInt16)(logListener == null ? 0 :((IPEndPoint)logListener.LocalEndpoint).Port);
			}
		}
		public delegate void NewLog(String logStr);
		public event NewLog NewLogEvent;
		private void doRecvLog(IAsyncResult ar)
		{
			Dictionary<string, object> readParams = ar.AsyncState as Dictionary<string, object>;
			TcpClient logClient = readParams["logClient"] as TcpClient;
			byte[] buffer = readParams["buffer"] as byte[];
			logClient.GetStream().EndRead(ar);
			Int32 size = BitConverter.ToInt32(readParams["size"] as byte[],0);
			if (size == 0) {
				Close();
				return;
			}
			String log_str = System.Text.Encoding.UTF8.GetString(buffer, 0, BitConverter.ToInt32((readParams["size"] as byte[]), 0) - 1);
			logClient.GetStream().BeginRead(buffer, 0, 4, doRecvSize, readParams);

			NewLogEvent(log_str);
			//Action<string> actionDelegate = (str) => { logPanel.AppendText(str); };
			//listBox.Dispatcher.Invoke(actionDelegate, log_str);
		}
	}
}