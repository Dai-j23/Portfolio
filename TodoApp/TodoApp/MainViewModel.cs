using CommunityToolkit.Mvvm.ComponentModel;
using CommunityToolkit.Mvvm.Input;
using System.Collections.ObjectModel;
using System.Diagnostics;
using System.Threading.Tasks;

namespace TodoApp
{
    public partial class MainViewModel : ObservableObject
    {
        private readonly TodoDatabase _database;

        // UIが自動で変更を検知できるリスト
        public ObservableCollection<TodoItem> Todos { get; } = new();

        // 入力欄のテキストと連動するプロパティ
        [ObservableProperty]
        private string _newTodoText = string.Empty; // 空文字列で初期化

        public MainViewModel(TodoDatabase database)
        {
            _database = database;
            LoadTodos(); // 起動時にデータを読み込む
        }

        private async void LoadTodos()
        {
            var todos = await _database.GetItemsAsync();
            if (todos != null)
            {
                Todos.Clear();
                Debug.WriteLine("--- Loading Todos ---"); // ← ログ出力1
                foreach (var todo in todos)
                {
                    Todos.Add(todo);
                    Debug.WriteLine($"Adding: {todo.Task}, Checked: {todo.IsChecked}"); // ← ログ出力2
                }
            }
        }

        // 「追加」ボタンが押された時に実行されるコマンド
        [RelayCommand]
        private async Task AddTodo()
        {
            if (string.IsNullOrWhiteSpace(NewTodoText)) return;

            var newTodo = new TodoItem { Task = NewTodoText };
            await _database.SaveItemAsync(newTodo);

            LoadTodos(); // リストに表示
            NewTodoText = string.Empty; // 入力欄を空にする
        }

        [RelayCommand]
        private async Task DeleteTodo(TodoItem todo)
        {
            if (todo == null) return;

            // データベースから削除
            await _database.DeleteItemAsync(todo);
            // 画面のリストからも削除
            LoadTodos();
        }

        [RelayCommand]
        private async Task UpdateTodoStatus(TodoItem todo)
        {
            if (todo == null) return;
            Debug.WriteLine($"--- UpdateTodoStatus called for: {todo.Task} ---"); // ← ログ出力3

            // データベース内のisCheckedの状態を更新
            await _database.SaveItemAsync(todo);
            // データベースからリストを再読み込みして、並び替えを反映させる
            LoadTodos();
        }
    }
}