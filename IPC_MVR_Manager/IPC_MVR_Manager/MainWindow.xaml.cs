using Newtonsoft.Json;
using Newtonsoft.Json.Linq;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
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
        ObservableCollection<string> clientIP = new ObservableCollection<string>();
        static Dictionary<string, JObject> clientMap = new Dictionary<string, JObject>();
        List<string> test = new List<string>();
        void onMulticastMsg(JObject obj) {
            if (!clientMap.ContainsKey(obj["server-ip"].ToString())) {
                clientMap[obj["server-ip"].ToString()] = obj;
                Action<string> actionDelegate = (x) => { clientIP.Add(x); };
                listBox.Dispatcher.Invoke(actionDelegate, obj["server-ip"].ToString());
            }
            
            JObject msg = new JObject();
            msg["c#-test"] = "123456";
            msg = client.Request("test-topic", msg);
            Console.WriteLine(" {0} 应答消息 {1}  ", obj["server-ip"], JsonConvert.SerializeObject(msg)); 

        }
        public MainWindow()
        {
            //client.Connect("192.168.10.74", 8098);
            client.ListenMultiCastMsg(onMulticastMsg);
            client.Start();
            InitializeComponent();
            listBox.ItemsSource = clientIP;
        }

        private void Button_Click(object sender, RoutedEventArgs e)
        {
            
            Console.WriteLine("==== {0} ", listBox.SelectedItem);
            client.Connect(listBox.SelectedItem.ToString(), clientMap[listBox.SelectedItem.ToString()].Value<UInt16>("port"));
        }
    }
}
