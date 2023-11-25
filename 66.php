<?php  
   function p($val)
   {
       // var_dump($val);
       var_export($val);
       echo "<br/>";
       // exit;
   }
   //矩形
class Rectangle {  
    public $x;  
    public $y;  
    public $width;  
    public $height;  
  
    public function __construct($x, $y, $width, $height) {  
        $this->x = $x;  
        $this->y = $y;  
        $this->width = $width;  
        $this->height = $height;  
    }  
  
    public function get_area() {  
        return $this->width * $this->height;  
    }  
}  
  //纸
class Paper {  
    public $width;  
    public $height;  
    public $rectangles;  
  //初始化纸张大小
    public function __construct($width, $height) {  
        $this->width = $width;  
        $this->height = $height;  
    }  
   //当前矩形的位置是否超出纸张范围
    public function is_valid($rectangle) {  
       
        return $rectangle->x + $rectangle->width <= $this->width &&  
            $rectangle->y + $rectangle->height <= $this->height;  
    }  
  //获取所有纸张内矩形面积
    public function get_remaining_area() {  
        // p($this->rectangles);
        return $this->width * $this->height - array_sum(
            array_map(function ($rectangle) {  
                
            return $rectangle->width * $rectangle->height;  
        }, 
        $this->rectangles));  
    }  
  
    public function add_rectangle($rectangle) {  
        if ($this->is_valid($rectangle)) {  
            $this->rectangles[] = $rectangle;  
        }  
    }  
}  
  
// 创建了一个名为GeneticSqueezingAlgorithm的类，该类包含了遗传算法的各个参数，包括种群大小（pop_size）、变异率（mutation_rate）、迭代次数（generations）和精英个体数量（elite_size）等。
// 类中定义了一个构造函数__construct，用于初始化类的各个参数。
// 类中定义了一个run方法，用于执行遗传算法的主要逻辑。
// 在run方法中，首先创建了一个Paper对象，用于表示纸张大小。
// 接着创建了一个空数组population，用于存储种群。
// 使用循环生成初始种群。每个个体都是由50个随机放置在纸张内的矩形组成。
// 使用循环迭代执行遗传算法的迭代过程。
// 在每次迭代中，首先使用轮盘赌选择法选择精英个体，并将它们的适应度值累加到total_fitness中。
// 使用随机选择法选择一个随机精英个体，并将其与当前迭代中的其他精英个体进行交换。
// 完成迭代后，返回适应度最高的个体作为结果。




class GeneticSqueezingAlgorithm {  
    public $pop_size;  //种群大小
    public $mutation_rate;   //变异率
    public $generations;  //迭代次数
    public $elite_size;  //和精英个体数量
  //初始化遗传算法参数
    public function __construct($pop_size, $mutation_rate, $generations, $elite_size) {  
        $this->pop_size = $pop_size;  
        $this->mutation_rate = $mutation_rate;  
        $this->generations = $generations;  
        $this->elite_size = $elite_size;  
    }  
  //开始遗传算法
    public function run() {  
        // 创建Paper对象，用于表示纸张大小 
        $paper_size = new Paper(889, 1194);  
        // 创建空数组，用于存储种群
        $population = [];  
        // 创建初始种群
        for ($i = 0; $i < $this->pop_size; $i++) {  
            $individual = [];  
            for ($j = 0; $j < 10; $j++) {  
                 // 生成随机坐标 
                $x = rand(0, $paper_size->width - 240);  
                 // 创建矩形对象  
                $y = rand(0, $paper_size->height - 299);  
                $individual[] = new Rectangle($x, $y, 240, 299);  
            }  
            $population[] = $individual;  
        }  
      
  // 迭代遗传算法的次数 
        for ($i = 0; $i < $this->generations; $i++) {  
            // Select parents using roulette wheel selection  
            // 选择父母使用轮盘赌选择法
            $parents = [];  
            $total_fitness = 0;  
            for ($j = 0; $j < $this->elite_size; $j++) {  
                $best_fitness = 0;  
                $best_individual = null;  
                  // 遍历种群，选择适应度最高的个体  
                foreach ($population as $individual) {  
                    //获取每个种群在纸张的适应度
                    $fitness = $this->get_fitness($individual, $paper_size);  
                 
                    //适应度替换
                    if ($fitness > $best_fitness) {  
                        $best_fitness = $fitness;  
                        $best_individual = $individual;  
                    }  
                }  
             
                //选择几个适应度最好的群新组
                $parents[] = $best_individual;  
                $total_fitness += $best_fitness;  
            }  
            p($parents);
            // 对父母进行随机选择 
            foreach ($parents as $k => $parent) {  
                $random_parent_index = rand(0, count($parents) - 1);  
                $swap = array_splice($parents, $random_parent_index, 1);  
                $parent = array_shift($swap);  
                $parents[$k] = $parent;  
            }  
           // 轮盘赌选择结束  
    // 交叉操作开始  
    
            for ($j = 0; $j < 20; $j++) {  
                $parent1 = $parents[rand(0, count($parents) - 1)];  
                $parent2 = $parents[rand(0, count($parents) - 1)];  
                $child = [];  
               
                for ($l = 0; $l < 50; $l++) {  
                     // 生成随机数，决定采用哪个父代基因
                    $coin = rand(0, 1);  
                    if ($coin > 0.5) {  
                         // 选择父代1的基因  
                        $child[] = $parent1[rand(0, count($parent1) - 1)];  
                    } else {  
                       
                         // 选择父代2的基因  
                        $child[] = $parent2[rand(0, count($parent2) - 1)];  
                    }  
                }  
                // 将生成的子代加入种群  
                $population[] = $child;  
            }  
  
            // Mutation  
//             这段代码包含了两个嵌套的循环，用于对种群中的每个个体进行变异操作。在每个循环中，先生成一个随机的变异概率，如果该概率小于指定的变异率，则对该个体进行变异。具体操作是，随机生成一个坐标偏移量，将其加到该个体的坐标上，然后创建一个新的矩形对象，用新的坐标和大小替换原来的矩形对象。

// 接下来的代码行使用array_filter函数对种群进行过滤，去除适应度小于等于0的个体。array_filter函数接受一个回调函数作为参数，该函数用于判断每个个体是否应该保留在种群中。在这个回调函数中，调用了get_fitness方法来计算个体的适应度值，如果适应度值大于0，则保留该个体，否则去除。
            for ($j = 0; $j < count($population); $j++) {  
                for ($k = 0; $k < count($population[$j]); $k++) {  
                    $mutation_chance = rand(0, 100);  
                    if ($mutation_chance < $this->mutation_rate) {  
                        $x = $population[$j][$k]->x;  
                        $y = $population[$j][$k]->y;  
                        $population[$j][$k] = new Rectangle($x + rand(-20, 20), $y + rand(-20, 20), 240, 299);  
                    }  
                }  
            }  
  
            $population = array_filter($population, function ($individual,$paper_size) {  
                return $this->get_fitness($individual, $paper_size) > 0;  
            });  
        }  
  
        return $population;  
    }  
    // 这段代码是一个计算适应度的方法。它接受两个参数，individual表示一个个体，即一个由矩形组成的数组，paper_size表示纸张大小。

    // 在方法内部，首先将适应度初始化为0。然后使用foreach循环遍历$individual中的每个矩形。在每次循环中，判断该矩形是否在纸张大小范围内，如果是，则计算该矩形的面积减去剩余面积，并将其加到适应度上。
    
    // 最后，将适应度取负并返回。这里取负的原因是为了将适应度值转化为最小化目标函数的形式，使得遗传算法可以更容易地找到最优解。
    public function get_fitness($individual, $paper_size) {  
        
        $fitness = 0;  
        foreach ($individual as $rectangle) {  
            if ($paper_size->is_valid($rectangle)) {  
                $paper_size->add_rectangle($rectangle);
                $remaining_area = $paper_size->get_remaining_area();  
                $fitness += $rectangle->get_area() - $remaining_area;  
            }  
        }  
        return -$fitness;  
    }  
}

// 在遗传算法中，适应度是指个体在种群中的适应程度，用于衡量个体在优化问题中的优劣程度。适应度函数通常用于评估个体的适应度，它将个体的基因表现转换为数值，以便进行比较和选择。

// 在遗传算法中，适应度函数通常被用来衡量个体在优化问题中的表现。这个函数接受个体的编码串作为输入，并返回一个实数值，表示该个体的适应度。适应度函数的值越大，表示该个体的适应度越高，也就是更优秀。

// 适应度函数的设计对遗传算法的性能和效果起着至关重要的作用。一个好的适应度函数应该能够准确评估个体的优劣，并且具有如下特点：单值、连续、非负、最大化、合理、一致性、计算量尽可能小、通用性尽可能强等。

// 在遗传算法的运行过程中，适应度函数会根据个体的编码串计算出其适应度，并根据适应度进行选择、交叉和变异等操作，以逐步接近优化问题的最优解










$pop_size = 13; // 种群大小  
$mutation_rate = 5; // 变异率  
$generations = 30; // 代数  
$elite_size = 2; // 精英个体数量  
  
$paper_size = new Paper(889, 1194); // 创建纸张对象  
  
$ga = new GeneticSqueezingAlgorithm($pop_size, $mutation_rate, $generations, $elite_size);  
$population = $ga->run(); // 运行遗传算法  
  
// 输出结果  
echo '最佳适应度个体数量：' . count($population) . "\n";  
foreach ($population as $individual) {  
    $fitness = $ga->get_fitness($individual, $paper_size);  
    echo '个体适应度：' . -$fitness . "\n";  
    foreach ($individual as $rectangle) {  
        echo '矩形坐标：(x=' . $rectangle->x . ', y=' . $rectangle->y . ')' . "\n";  
    }  
    echo "\n";  
}  


// 在这段代码中，没有提到$paper对象调用add_rectangle方法。也许您在查看的是不完整的代码，或者是有其他地方调用了add_rectangle方法来添加矩形到$paper对象中。

// 一般来说，$paper对象应该包含一个add_rectangle方法，用于将矩形添加到纸张中。这样，在计算适应度时，就可以通过调用$paper->is_valid($rectangle)方法来检查矩形是否在纸张范围内，并且可以通过调用$paper->get_remaining_area()方法来获取剩余面积。