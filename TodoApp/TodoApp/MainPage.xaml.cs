namespace TodoApp
{
    public partial class MainPage : ContentPage
    {
        // コンストラクタでViewModelを受け取るように変更
        public MainPage(MainViewModel viewModel)
        {
            InitializeComponent();

            // このページの司令塔(BindingContext)はviewModelです、と設定
            BindingContext = viewModel;
        }
    }
}