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
using System.Configuration;

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
            InitializeComponent();
            //client.Connect("192.168.10.74", 8098);
            client.ListenMultiCastMsg(onMulticastMsg);
            client.ListenClose(onClientClose);
            client.Start();

            
            
            listBox.ItemsSource = clientIP;
            String str = ConfigurationManager.AppSettings["TmpPath"];
            if (!System.IO.File.Exists(AppDomain.CurrentDomain.BaseDirectory + "\\app.config"))
            {
                return;
            }

            Configuration cfa = ConfigurationManager.OpenExeConfiguration(ConfigurationUserLevel.None);

            if (cfa.AppSettings.Settings.AllKeys.Contains("mcu-file-bin"))
            {
                textMCUFile.Text = cfa.AppSettings.Settings["mcu-file-bin"].Value.ToString();
            }
            if (cfa.AppSettings.Settings.AllKeys.Contains("fpga-file-bin"))
            {
                textFPGAFile.Text = cfa.AppSettings.Settings["fpga-file-bin"].Value.ToString();
            }
            if (cfa.AppSettings.Settings.AllKeys.Contains("input-addr"))
            {
                textIP.Text = cfa.AppSettings.Settings["input-addr"].Value.ToString();
            }
            

            cfa.Save();

        }

        private void Button_Click(object sender, RoutedEventArgs e)
        {
    
            if (client.IsConnected)
                client.Close();

            if (radioBtnInput.IsChecked == true)
            {
                string[] addr = textIP.Text.Split(':');
                if (addr.Length < 2)
                {
                    MessageBox.Show("输入地址有误，请输入正确的地址如 192.168.10.2:8888");
                    return;
                }
                try
                {
                    client.Connect(addr[0].ToString(), UInt16.Parse(addr[1]));
                }
                catch {
                    MessageBox.Show("无法连接，请输入正确的地址如 192.168.10.2:8888");
                    label.Foreground = new SolidColorBrush(Colors.Black);
                    label.Content = "连接到:" + addr[0].ToString();
                    return;
                }
                
                if (client.IsConnected)
                {
                    JObject obj = client.Request("query", null);
                    obj["msg"] = obj.Value<JObject>("reply");
                    clientMap[addr[0].ToString()] = obj;
                    mCurrentDev = obj.Value<JObject>("reply");
                    updateInfo();
                    label.Foreground = new SolidColorBrush(Colors.Black);
                    label.Content = "连接到:" + addr[0].ToString();
                }
                else
                {
                    label.Foreground = new SolidColorBrush(Colors.Red);
                    label.Content = "未连接";
                    MessageBox.Show("无法连接，请输入正确的地址如 192.168.10.2:8888");
                    return;
                }
                Configuration cfa = ConfigurationManager.OpenExeConfiguration(ConfigurationUserLevel.None);
                cfa.AppSettings.Settings["input-addr"].Value = textIP.Text;
                cfa.Save();
                return;
            }
            else {
                if (listBox.SelectedItem == null)
                {
                    MessageBox.Show("请从列表选择合适的地址");
                    return;
                }
                client.Connect(listBox.SelectedItem.ToString(), clientMap[listBox.SelectedItem.ToString()].Value<UInt16>("port"));
                if (client.IsConnected)
                {
                    label.Foreground = new SolidColorBrush(Colors.Black);
                    label.Content = "连接到:" + listBox.SelectedItem.ToString();
                    mCurrentDev = clientMap[listBox.SelectedItem.ToString()];
                }
                else
                {
                    label.Foreground = new SolidColorBrush(Colors.Red);
                    label.Content = "未连接";
                    
                    MessageBox.Show("设备无法连接");
                    return;
                }
            }
            
            
        }
        private void onClientClose() {
            label.Foreground = new SolidColorBrush(Colors.Red);
            label.Content = "未连接";
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

                Configuration cfa = ConfigurationManager.OpenExeConfiguration(ConfigurationUserLevel.None);
                cfa.AppSettings.Settings["mcu-file-bin"].Value = textMCUFile.Text;
                cfa.Save();
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

                Configuration cfa = ConfigurationManager.OpenExeConfiguration(ConfigurationUserLevel.None);
                cfa.AppSettings.Settings["fpga-file-bin"].Value = textFPGAFile.Text;
                cfa.Save();
            }

        }


        private void Button_Click_3(object sender, RoutedEventArgs e)
        {
            string file_path = textMCUFile.Text;
            System.IO.FileInfo fileInfo = new System.IO.FileInfo(file_path);
            int size = (int)fileInfo.Length;
            JObject msg = new JObject();
            msg["file-name"] = "/usr/Hseries/configs/upgrade.tar";
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
                f.Close();
            }
        }

        private void Button_Click_4(object sender, RoutedEventArgs e)
        {
            string file_path = textFPGAFile.Text;
            System.IO.FileInfo fileInfo = new System.IO.FileInfo(file_path);
            int size = (int)fileInfo.Length;
            JObject msg = new JObject();
            if(mCurrentDev.Value<string>("dev") == "mvr")
                msg["file-name"] = "/usr/Hseries/configs/mvr_fpga.bin";
            else
                msg["file-name"] = "/usr/Hseries/configs/ipc_fpga.bin";
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
                f.Close();
            }
        }
        void updateInfo() {
            if (mCurrentDev == null)
            {
                textDev.Text = "";
                textMCU.Text = "";
                textFPGA.Text = "";
                textSlotId.Text = "";
            }
            else {
                textDev.Text = mCurrentDev.Value<String>("dev");
                textMCU.Text = mCurrentDev.Value<String>("mcu-ver");
                textFPGA.Text = mCurrentDev.Value<String>("fpga-ver");
                textSlotId.Text = mCurrentDev.Value<String>("slot-id");
            }
            
        }
        private void listBox_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            if (listBox.SelectedItem != null) {
                string dev_ip = listBox.SelectedItem.ToString();
                mCurrentDev = clientMap[dev_ip].Value<JObject>("msg");
                updateInfo();
            }
            
        }

        private void Button_Click_Reboot(object sender, RoutedEventArgs e)
        {
            if (client.IsConnected)
            {
                client.Request("reboot", null);
            }
        }

        private void btnClickClearDevList(object sender, RoutedEventArgs e)
        {
            clientMap.Clear();
            clientIP.Clear();
        }
    }
}