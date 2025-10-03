-- |
-- Main module to interactively build and continuously search a Binary Search Tree.
-- This version includes robust error handling for user input.
--
import BinarySearchTree ( Tree, emptyTree, insert, search, inOrderTraversal )
import Data.Foldable ( foldl' )
import System.IO ( hFlush, stdout )
import Text.Read ( readMaybe ) -- 提案1: 安全な読み込みのために readMaybe をインポート

-- | 構築された木に対して、連続して検索を行うためのループ関数
searchLoop :: Tree Int -> IO ()
searchLoop tree = do
    putStr "\n検索したい数値を入力してください（終了するには 'q' を入力）: "
    hFlush stdout
    input <- getLine

    -- 提案1: readMaybe を使った例外対策を実装
    case input of
        "q"    -> putStrLn "プログラムを終了します。お疲れ様でした！"
        "quit" -> putStrLn "プログラムを終了します。お疲れ様でした！"
        _      -> case readMaybe input :: Maybe Int of
            -- 入力が数値でない場合
            Nothing -> do
                putStrLn "エラー: 有効な数値を入力してください。"
                searchLoop tree -- 再度入力を促す
            -- 入力が数値の場合
            Just searchValue -> do
                let found = search searchValue tree
                -- 提案1の応用: if-then-else式を使って表示を一行にまとめる
                let resultMessage = if found then "は見つかりました。" else "は見つかりませんでした。"
                putStrLn $ "結果: 「" ++ show searchValue ++ "」" ++ resultMessage
                searchLoop tree -- 次の検索のためにループ

-- | メインの処理
main :: IO ()
main = do
    -- リストの入力と木の構築
    putStr "スペース区切りで数値のリストを入力してください (例: 5 2 8 1 9): "
    hFlush stdout
    line <- getLine
    let numStrings = words line
    let nums = map read numStrings :: [Int]

    let tree = foldl' (flip insert) emptyTree nums

    -- 提案2: print tree の代わりに、ソート済みの分かりやすいリストを表示
    putStrLn "\n--- 木を構築しました ---"
    putStrLn $ "木の要素（ソート済み）: " ++ show (inOrderTraversal tree)

    -- 構築した木を引数として、検索ループを開始する
    searchLoop tree