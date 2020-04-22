using Newtonsoft.Json;
using Newtonsoft.Json.Linq;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;


namespace IPC_MVR_Manager
{
    /// <summary>
    /// MainWindow.xaml 的交互逻辑
    /// </summary>
    public partial class MainWindow : Window
    {
        static STPClient.STPClient client = new STPClient.STPClient("224.0.10.200", 8888);
        static Dictionary<string, string> clientMap = new Dictionary<string, string>();
        List<string> test = new List<string>();
        void onMulticastMsg(JObject obj) {
            Console.WriteLine("222 {0} 传来消息 {1}  ", obj["server-ip"],  obj["msg"]);
            clientMap[obj["server-ip"].ToString()] =  obj["server-ip"].ToString();
            JObject msg = new JObject();
            msg["c#-test"] = "123456";
            msg = client.Request("test-topic", msg);
            Console.WriteLine(" {0} 应答消息 {1}  ", obj["server-ip"], JsonConvert.SerializeObject(msg));

        }
        public MainWindow()
        {
            client.Connect("192.168.10.74", 8098);
            client.ListenMultiCastMsg(onMulticastMsg);
            client.Start();
            InitializeComponent();
            listBox.ItemsSource = clientMap;
        }
    }
}
