using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using System.Net;
using System.Net.Sockets;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;

namespace IPC_MVR_Manager.STPClient
{
	class STPClient
	{
		int mSeq = 0;
		UdpClient mMulticastSocket = null;
		TcpClient mClientSocket = null;
		public STPClient(String multicastAddr, UInt16 multicastPort)
		{
			mMulticastSocket = new UdpClient();
			IPAddress ipaddr = IPAddress.Parse(multicastAddr);
			if (ipaddr == IPAddress.Broadcast)
			{
				mMulticastSocket.Client.SetSocketOption(SocketOptionLevel.Socket, SocketOptionName.Broadcast, 1);
			}
			else
			{
				mMulticastSocket.Client.SetSocketOption(SocketOptionLevel.Socket, SocketOptionName.ReuseAddress, 1);
				mMulticastSocket.JoinMulticastGroup(IPAddress.Parse(multicastAddr), IPAddress.Any);
				//mMulticastSocket.JoinMulticastGroup(IPAddress.Parse(multicastAddr), IPAddress.Parse("192.168.10.74"));
			}
			//



			mMulticastSocket.Client.Bind(new IPEndPoint(IPAddress.Any, multicastPort));

			
		}
		public bool IsConnected {
			get {
				return mClientSocket != null && mClientSocket.Connected;
			}
			
		}
		public void Connect(string ipaddr,UInt16 port) {
			//EndPoint serverAddr = new IPEndPoint(IPAddress.Parse(ipaddr), port);
			try
			{
				mClientSocket = new TcpClient();
				mClientSocket.Connect(ipaddr, port);
			}

			catch (Exception e)
			{
				Console.WriteLine("Error:"+e.ToString());
			}
			
		}
		public void Close()
		{
			//EndPoint serverAddr = new IPEndPoint(IPAddress.Parse(ipaddr), port);
			if (!IsConnected)
				return;
			if(mClientSocket != null)
				mClientSocket.Close();
			mClientSocket = null;

		}
		public void WriteBinary(byte[] data, int offset, int count) {
			byte[] buff = new byte[count + 8];
			Array.Copy(BitConverter.GetBytes((UInt32)(count + 4)), buff, 4);
			buff[4] = 1;
			Array.Copy(data, offset, buff, 8, count);
			mClientSocket.GetStream().Write(buff, 0, buff.Length);
		}
		public void WriteBinary(byte[] data) {
			WriteBinary(data, 0, data.Length);
		}
		public JObject Request(string topic,JObject msg)
		{
			if (!mClientSocket.Connected)
				return null;
			
			JObject obj = new JObject();
			obj["topic"] = topic;
			if (msg != null)
				obj["msg"] = msg;
			obj["seq"] = mSeq++;
			string str = JsonConvert.SerializeObject(obj);
			byte[] bytes = System.Text.Encoding.UTF8.GetBytes(str);
			byte[] buff = new byte[bytes.Length + 9];

			Array.Copy( BitConverter.GetBytes(buff.Length - 4),buff,4);
			buff[4] = 0;
			Array.Copy(bytes, 0,buff,8, bytes.Length);
			buff[buff.Length-1] = 0;

			mClientSocket.GetStream().Write(buff, 0,buff.Length);
			buff = new byte[4];
			if (mClientSocket.GetStream().Read(buff, 0, 4) == 0)
			{
				return null;
			}
			int pending_size = BitConverter.ToInt32(buff, 0);
			buff = new byte[pending_size];
			int offset = 0;
			while (pending_size > 0) {
				int count = mClientSocket.GetStream().Read(buff, offset, pending_size);
				if (count == 0) {
					return null;
				} 
				offset += count;
				pending_size -= count;
			}

			obj = (JObject)JsonConvert.DeserializeObject(System.Text.Encoding.UTF8.GetString(buff,0,buff.Length-1));
			return obj;
		}
		public void Start()
		{
			byte[] buff = new byte[64 * 1024];
			Dictionary<String, Object> map = new Dictionary<string, object>();
			EndPoint senderIP = new IPEndPoint(IPAddress.Any, 0);
			map["server-ip"] = senderIP;
			map["recv-buffer"] = buff;
			mMulticastSocket.BeginReceive(OnDataReceived, map);
			//mMulticastSocket.BeginReceiveFrom(buff, 0, buff.Length, SocketFlags.None, ref senderIP, OnDataReceived, map);
		}
		public delegate void OnMultiCastMsg(JObject name);
		private OnMultiCastMsg onMultiCastMsg = null;
		public void ListenMultiCastMsg(OnMultiCastMsg onMultiCastMsg) {
			this.onMultiCastMsg = onMultiCastMsg;
		}
		public void OnDataReceived(IAsyncResult ar)
		{
			Dictionary<String, Object> map = ar.AsyncState as Dictionary<String, Object>;
			IPEndPoint senderIP = new IPEndPoint(IPAddress.Any, 0);
			// = map["recv-buffer"] as byte[];
			byte[] buff = mMulticastSocket.EndReceive(ar, ref senderIP);
			//System.Text.Encoding.UTF8.GetString(buff);
			//server-ip
			JObject obj = (JObject)JsonConvert.DeserializeObject(System.Text.Encoding.UTF8.GetString(buff, 0, buff.Length-1));
			obj["server-ip"] = senderIP.Address.ToString();
			if (onMultiCastMsg != null) {
				onMultiCastMsg(obj);
			}
			mMulticastSocket.BeginReceive( OnDataReceived, map);
		}
	};
}
	
	