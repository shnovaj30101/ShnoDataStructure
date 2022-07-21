# ShnoDataStructure

這個專案主要是針對比較複雜的資料結構做實作的練習，主要使用的語言是 C 和 C++。

除了學習各種資料結構的原理以及性質，還希望能藉此熟悉 C/C++ 的基本程式技巧和環境佈署，包括：

* Makefile 基本使用方法
* gdb debug 技巧
* Cmake 佈署流程與觀念
* C++ 第三方 library 以及 STL 常用模組使用
* DBMS 設計原理與結構

以下小節為該專案實作的資料結構的基本原理。

### RBTree

RBTree 中文稱作紅黑樹，因為其結構中的內部節點必為紅色或黑色而得名，紅黑樹是一個能夠保持高度平衡的 binary tree，這也是其內部節點必須著色的原因。

紅黑樹雖然不像 AVL tree 是嚴格平衡的，但也因此他所為了平衡所需要的旋轉次數遠比 AVL tree 來的低（三次以內），所以紅黑樹在操作的效率會比較高。

紅黑數雖然不是嚴格平衡的，但最高高度絕對不會高於最低高度的 2 倍，為了維持這樣的特性，紅黑樹必須遵守以下性質：

1. 節點必須為紅色或是黑色
2. 根節點必為黑色
3. 插入的節點必為紅色
4. 紅色的節點不能彼此為父子關係
5. 每個從根走到葉子節點的路徑，其中黑色節點數量，也就是黑色高度，必須一樣

一般網路上的紅黑樹會在葉子節點下面在插入黑色的 NIL 節點，也就是說，一般的紅黑樹的葉子節點全都是黑色的 NIL 節點。

但因為實作黑色 NIL 節點我覺得有點麻煩，所以我的紅黑樹不包含這個部分。

另外針對刪除節點的處理我也做了一些不一樣角度的實作，最後結果也能滿足所有紅黑樹的必要特性。

### BTree

BTree 主要的應用場景在於資料庫的核心結構，不像 RBTree 每一個 Node 只包含一筆資料，BTree 的一個 Node 可包含多筆資料，且 BTree 的 Node 可以有多個 child，而 RBTree 的 child 只包含 right child 和 left child。

當插入同樣的 dataset 到 RBTree 和 BTree 之中，BTree 的高度顯然會比 RBTree 來的低很多（因為 Btree每一層可容納的資料量比 RBTree 還要多），高度低的優勢是每次搜尋一筆資料的時候，BTree 需要經過比較少的 Node。

因為在資料庫的應用中，搜尋時經過的 Node 數等同於要從硬碟中讀取 data 的次數，因為要從硬碟中讀取資料的速度比 CPU 的運算速度還要慢的多，所以在尋找資料的時候，要盡可能減少從硬碟中讀取 data 的次數，所以 BTree 才適合應用在資料庫的核心中。 

### BTree_with_disk
