<?php  
  function p($val)
  {
      // var_dump($val);
      var_export($val);
      echo "<br/>";
      // exit;
  }
  class Individual {  
    public $individual;  
    public $fitness;  
  
    public function __construct($individual) {  
        $this->individual = $individual;  
        $this->fitness = PHP_INT_MAX;  
    }  
  
    public function calculateFitness() {  
        // 计算个体的适应度，即放置所有矩形所需的最小纸张面积  
        $x_count = count($this->individual);  
        $y_count = 0;  
        $best_fitness = PHP_INT_MAX;  
  
        for ($i = 0; $i < $x_count; $i++) {  
            $x1 = $this->individual[$i];  
            $y1 = 0;  
            $width = 240;  
            $height = 299;  
  
            for ($j = $i; $j >= 0 && $x1 + $width <= 889; $j--) {  
                $x2 = $x1 + $width;  
                $y2 = $y1 + $height;  
  
                $overlap = max(0, min($x2 - 889, 889 - $x1)) * max(0, min($y2 - 1194, 1194 - $y1));  
                $best_fitness = min($best_fitness, $overlap);  
  
                $y1 = max(0, $y2 - 1194);  
                $x1 = max(0, $x2 - 889);  
            }  
        }  
  
        $this->fitness = $best_fitness;  
        return $this->fitness;  
    }  
}
class Population {  
    // 定义个体数组和适应度  
    public $individuals;  
    public $fitness;  
  
    // 构造函数，初始化种群  
    public function __construct($size, $chromosome_length) {  
        $this->individuals = array_fill(0, $size, new Individual(str_split("0", $chromosome_length)));  
        $this->fitness = array_fill(0, $size, PHP_INT_MAX);  
    }  
    // 计算个体的适应度，即放置所有矩形所需的最小纸张面积  
   
    // 选择操作，使用轮盘赌算法选择个体  
    public function selection() {  
        // 计算个体适应度概率  
        $fitness_probabilities = array();  
        $total_fitness = 0;  
  
        for ($i = 0; $i < count($this->individuals); $i++) {  
            $fitness = $this->individuals[$i]->fitness;  
            $total_fitness += $fitness;  
            $fitness_probabilities[$i] = $fitness / $total_fitness;  
        }  
  
        // 生成选择概率分布的随机数序列  
        $rand_seq = array_fill(0, count($this->individuals), 0);  
        for ($i = 0; $i < count($this->individuals); $i++) {  
            $rand_seq[$i] = rand(0, 1000) / 1000;  
        }  
  
        // 根据概率选择个体  
        $selected_individuals = array();  
  
        for ($i = 0; $i < count($this->individuals); $i++) {  
            while ($rand_seq[$i] > $fitness_probabilities[$i]) {  
                $rand_seq[$i] -= $fitness_probabilities[$i];  
                $i++;  
                if ($i >= count($this->individuals)) {  
                    $i = 0;  
                }  
            }  
            $selected_individuals[] = $this->individuals[$i];  
            $rand_seq[$i] = 1; // 被选择的个体，随机数设为1，保证不会再次被选择  
        }  
  
        $this->individuals = $selected_individuals;  
    }  
  
    // 交叉操作，使用单点交叉  
    public function crossover() {  
        $new_individuals = array();  
  
        for ($i = 0; $i < count($this->individuals); $i += 2) {  
            $parent1 = $this->individuals[$i];  
            $parent2 = $this->individuals[$i + 1];  
            $child1 = array();  
            $child2 = array();  
  
            $crossover_point = rand(0, count($parent1->individual) - 1);  
  
            for ($j = 0; $j < count($parent1->individual); $j++) {  
                if ($j < $crossover_point) {  
                    $child1[] = $parent1->individual[$j];  
                } elseif ($j > $crossover_point) {  
                    $child1[] = $parent2->individual[$j];  
                } else {  
                    $child2[] = $parent2->individual[$j];  
                }  
            }  
  
            for ($j = 0; $j < count($parent2->individual); $j++) {  
                if (in_array($parent2->individual[$j], $child1)) {  
                    continue; // 避免重复基因  
                }  
                $child2[] = $parent2->individual[$j];  
            }  
  
            $new_individuals[] = new Individual($child1);  
            $new_individuals[] = new Individual($child2);  
        }  
  
        $this->individuals = $new_individuals;  
    }
    // 变异操作，使用随机基因位点变异  
    public function mutation() {  
        for ($i = 0; $i < count($this->individuals); $i++) {  
            if (rand(0, 1000) / 1000 < 0.5) {  
                $gene_index = rand(0, count($this->individuals[$i]->individual) - 1);  
                $this->individuals[$i]->individual[$gene_index] = ($this->individuals[$i]->individual[$gene_index] + rand(-5, 5)) % 889;  
            }  
        }  
    }  
  
    // 将种群转换为字符串  
    public function __toString() {  
        $str = "";  
        foreach ($this->individuals as $individual) {  
            $str .= implode(",", $individual->individual) . "\n";  
        }  
        return $str;  
    }  
}
// 主函数  
function main() {  
    $population_size = 100;  
    $chromosome_length = 20;  
    $max_generations = 1000;  
    $population = new Population($population_size, $chromosome_length);  
  
    $best_fitness = PHP_INT_MAX;  
    $best_individual = null;  
  
    for ($i = 0; $i < $max_generations; $i++) {  
        $population->selection();  
        $population->crossover();  
        $population->mutation();  
  
        $new_fitness = $population->calculateFitness();
        if ($new_fitness < $best_fitness) {  
            $best_fitness = $new_fitness;  
            $best_individual = $population->individuals[0];  
        }  
    }  
  
    echo "最佳适应度：".$best_fitness."\n";  
    echo "最佳个体：".implode(",", $best_individual->individual)."\n";  
    echo "最佳个体的坐标：";  
  
    for ($i = 0; $i < count($best_individual->individual); $i++) {  
        echo "(".$best_individual->individual[$i].",".($i + 1).") ";  
    }  
  
    echo "\n";  
}  
  
main();


// 这个代码使用了遗传算法和挤压算法来求解最大数量的最省纸排法，它通过模拟个体的选择、交叉和突变操作来不断优化解，最终得到最优解。在代码中，个体表示为一个由240299规格的矩形在8891194的纸上的坐标序列，通过计算个体适应度（即放置所有矩形所需的最小纸张面积）来评估个体的优劣。使用轮盘赌算法选择个体，并使用单点交叉和随机突变来进行进化操作。

// 运行这个代码将会输出最佳适应度、最佳个体以及每个矩形的坐标，即最优解。请注意，这个代码只是一个示例实现，具体的参数设置和算法细节可能需要根据你的问题进行修改和优化。