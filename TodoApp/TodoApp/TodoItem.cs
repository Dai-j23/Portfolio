using SQLite;
using System.ComponentModel.DataAnnotations;
using System.ComponentModel.DataAnnotations.Schema;

namespace TodoApp
{
    [SQLite.Table("todos")] // データベースのテーブル名を指定
    public class TodoItem
    {
        [PrimaryKey, AutoIncrement] // 主キーで自動採番
        public int Id { get; set; }

        [SQLite.MaxLength(200)]
        public string Task { get; set; }

        public bool IsChecked { get; set; }
    }
}