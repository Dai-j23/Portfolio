using SQLite;
using System.Collections.Generic;
using System.Threading.Tasks;

namespace TodoApp
{
    public class TodoDatabase
    {
        private readonly SQLiteAsyncConnection _database;

        public TodoDatabase()
        {
            // アプリのローカルフォルダにDBファイルを作成
            var dbPath = Path.Combine(FileSystem.AppDataDirectory, "TodoSQLite.db3");
            _database = new SQLiteAsyncConnection(dbPath);
            _database.CreateTableAsync<TodoItem>().Wait(); // テーブルがなければ作成
        }

        // 全てのToDoアイテムを取得
        public Task<List<TodoItem>> GetItemsAsync()
        {
            return _database.Table<TodoItem>().ToListAsync();
        }

        // ToDoアイテムを保存 (新規追加 or 更新)
        public Task<int> SaveItemAsync(TodoItem item)
        {
            if (item.Id != 0)
            {
                return _database.UpdateAsync(item);
            }
            else
            {
                return _database.InsertAsync(item);
            }
        }

        // ToDoアイテムを削除
        public Task<int> DeleteItemAsync(TodoItem item)
        {
            return _database.DeleteAsync(item);
        }
    }
}