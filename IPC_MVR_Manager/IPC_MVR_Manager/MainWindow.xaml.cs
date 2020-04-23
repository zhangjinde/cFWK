using Newtonsoft.Json;
using Newtonsoft.Json.Linq;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.IO;
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
        //static STPClient.STPClient client = new STPClient.STPClient("255.255.255.255", 8888);
        ObservableCollection<string> clientIP = new ObservableCollection<string>();
        static Dictionary<string, JObject> clientMap = new Dictionary<string, JObject>();
        List<string> test = new List<string>();
        JObject mCurrentDev = null;
        void onMulticastMsg(JObject obj)
        {
            if (!clientMap.ContainsKey(obj["server-ip"].ToString()))
            {
                clientMap[obj["server-ip"].ToString()] = obj;
                Action<string> actionDelegate = (x) => { clientIP.Add(x); };
                listBox.Dispatcher.Invoke(actionDelegate, obj["server-ip"].ToString());
            }

            //JObject msg = new JObject();
            //msg["c#-test"] = "123456";
            //msg = client.Request("test-topic", msg);
            //Console.WriteLine(" {0} 应答消息 {1}  ", obj["server-ip"], JsonConvert.SerializeObject(msg));

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
            if (listBox.SelectedItem == null)
                return;
            if (client.IsConnected)
                client.Close();
            client.Connect(listBox.SelectedItem.ToString(), clientMap[listBox.SelectedItem.ToString()].Value<UInt16>("port"));
            if (client.IsConnected)
            {
                label.Foreground = new SolidColorBrush(Colors.Black);
                label.Content = "连接到:" + listBox.SelectedItem.ToString();
                mCurrentDev = clientMap[listBox.SelectedItem.ToString()];
            }
            else {
                label.Foreground = new SolidColorBrush(Colors.Red);
                label.Content = "未连接";
            }
            
        }

        private void Button_Click_1(object sender, RoutedEventArgs e)
        {
            System.Windows.Forms.OpenFileDialog openFileDialog1 = new System.Windows.Forms.OpenFileDialog();
            openFileDialog1.InitialDirectory = "c:\\";
            openFileDialog1.Filter = "txt files (*.txt)|*.txt|All files (*.*)|*.*";
            openFileDialog1.FilterIndex = 2;
            openFileDialog1.RestoreDirectory = true;
            if (openFileDialog1.ShowDialog() == System.Windows.Forms.DialogResult.OK)
            {
                textMCUFile.Text = openFileDialog1.FileName;
                //此处做你想做的事 ...=openFileDialog1.FileName; 
            }

        }
        private void Button_Click_2(object sender, RoutedEventArgs e)
        {
            System.Windows.Forms.OpenFileDialog openFileDialog1 = new System.Windows.Forms.OpenFileDialog();
            openFileDialog1.InitialDirectory = "c:\\";
            openFileDialog1.Filter = "txt files (*.txt)|*.txt|All files (*.*)|*.*";
            openFileDialog1.FilterIndex = 2;
            openFileDialog1.RestoreDirectory = true;
            if (openFileDialog1.ShowDialog() == System.Windows.Forms.DialogResult.OK)
            {
                textFPGAFile.Text = openFileDialog1.FileName;
                //此处做你想做的事 ...=openFileDialog1.FileName; 
            }

        }


        private void Button_Click_3(object sender, RoutedEventArgs e)
        {
            string file_path = textMCUFile.Text;
            System.IO.FileInfo fileInfo = new System.IO.FileInfo(file_path);
            int size = (int)fileInfo.Length;
            JObject msg = new JObject();
            msg["file-name"] = "/usr/Hseries/config/upgrade.tar";
            msg["file-len"] = size;
            JObject ack = client.Request("file-upload", msg);
            if (ack != null && ack["reply"]["ack"].ToString() == "ok")
            {
                FileStream f = new FileStream(file_path, FileMode.Open);
                byte[] buff = new byte[1024 * 60];
                while (size > 0)
                {
                    int len = size > buff.Length ? buff.Length : size;
                    int count = f.Read(buff, 0, len);
                    client.WriteBinary(buff, 0, count);
                    size -= count;
                }
            }
        }

        private void Button_Click_4(object sender, RoutedEventArgs e)
        {
            string file_path = textFPGAFile.Text;
            System.IO.FileInfo fileInfo = new System.IO.FileInfo(file_path);
            int size = (int)fileInfo.Length;
            JObject msg = new JObject();
            if(mCurrentDev.Value<string>("dev") == "mvr")
                msg["file-name"] = "/usr/Hseries/config/mvr_fpga.bin";
            else
                msg["file-name"] = "/usr/Hseries/config/ipc_fpga.bin";
            msg["file-len"] = size;
            JObject ack = client.Request("file-upload", msg);
            if (ack != null && ack["reply"]["ack"].ToString() == "ok")
            {
                FileStream f = new FileStream(file_path, FileMode.Open);
                byte[] buff = new byte[1024 * 60];
                while (size > 0)
                {
                    int len = size > buff.Length ? buff.Length : size;
                    int count = f.Read(buff, 0, len);
                    client.WriteBinary(buff, 0, count);
                    size -= count;
                }
            }
        }

        private void listBox_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            string dev_ip = listBox.SelectedItem.ToString();
            JObject obj = clientMap[dev_ip].Value<JObject>("msg");
            textDev.Text = obj.Value<String>("dev");
            textMCU.Text = obj.Value<String>("mcu-ver");
            textFPGA.Text = obj.Value<String>("fpga-ver");
            textSlotId.Text = obj.Value<String>("slot-id");

        }

        private void Button_Click_Reboot(object sender, RoutedEventArgs e)
        {
            if (client.IsConnected)
            {
                client.Request("reboot", null);
            }
        }
    }
}