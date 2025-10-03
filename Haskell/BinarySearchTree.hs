-- |
-- Module      : BinarySearchTree
-- Description : A module for a generic Binary Search Tree.
--
module BinarySearchTree
    ( Tree(..)      -- Tree data type (EmptyTree, Node)
    , emptyTree     -- An empty tree constant
    , insert        -- Function to insert an element
    , search        -- Function to search for an element
    , inOrderTraversal -- Renamed from traverse
    ) where

-- | A Binary Search Tree containing elements of type 'a'.
-- The 'Show' instance allows the tree to be printed for debugging.
data Tree a = EmptyTree | Node a (Tree a) (Tree a) deriving (Show)

-- | Represents an empty tree.
emptyTree :: Tree a
emptyTree = EmptyTree

-- | Inserts an element into the tree.
-- If the element already exists, the tree remains unchanged.
-- Requires the element type to be an instance of the Ord typeclass for comparison.
insert :: Ord a => a -> Tree a -> Tree a
insert x EmptyTree = Node x EmptyTree EmptyTree
insert x (Node a left right)
    | x == a    = Node a left right
    | x < a     = Node a (insert x left) right
    | otherwise = Node a left (insert x right)

-- | Searches for an element in the tree.
-- Returns True if the element is found, otherwise False.
search :: Ord a => a -> Tree a -> Bool
search _ EmptyTree = False
search x (Node a left right)
    | x == a    = True
    | x < a     = search x left
    | otherwise = search x right

-- | Performs an in-order traversal of the tree.
-- This results in a list of elements sorted in ascending order.
inOrderTraversal :: Tree a -> [a]
inOrderTraversal EmptyTree = []
inOrderTraversal (Node a left right) = inOrderTraversal left ++ [a] ++ inOrderTraversal right