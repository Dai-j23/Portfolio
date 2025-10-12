namespace TodoApp
{
    public partial class MainPage : ContentPage
    {
        public MainPage(MainViewModel viewModel)
        {
            InitializeComponent();
            BindingContext = viewModel;
        }

        // ↓↓↓ このメソッドを丸ごと追加 ↓↓↓
        private void TodoCheckBox_CheckedChanged(object sender, CheckedChangedEventArgs e)
        {
            // 1. イベントの発生源であるCheckBoxを取得
            var checkBox = (CheckBox)sender;
            // 2. CheckBoxに紐づいているTodoItemデータを取得
            var todoItem = (TodoItem)checkBox.BindingContext;
            // 3. このページのViewModelを取得
            var viewModel = (MainViewModel)this.BindingContext;

            // 4. ViewModelの更新コマンドを実行
            if (viewModel.UpdateTodoStatusCommand.CanExecute(todoItem))
            {
                viewModel.UpdateTodoStatusCommand.Execute(todoItem);
            }
        }
    }
}