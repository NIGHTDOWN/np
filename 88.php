<?php
function p($val)
{
    // var_dump($val);
    var_export($val);
    echo "<br/>";
    // exit;
}

class Rectangle
{
    public $x;
    public $y;
    public $width;
    public $height;

    public function __construct($x, $y, $width, $height)
    {
        $this->x = $x;
        $this->y = $y;
        $this->width = $width;
        $this->height = $height;
    }
}

class GSASolver
{
    private $populationSize;
    private $rectangleCount;
    private $rectangleWidth;
    private $rectangleHeight;
    private $paperWidth;
    private $paperHeight;
    private $population;
    private $fitnesses;
    private $bestFitness;
    private $generationCount;

    public function __construct($populationSize, $rectangleCount, $rectangleWidth, $rectangleHeight, $paperWidth, $paperHeight)
    {
        $this->populationSize = $populationSize;
        $this->rectangleCount = $rectangleCount;
        $this->rectangleWidth = $rectangleWidth;
        $this->rectangleHeight = $rectangleHeight;
        $this->paperWidth = $paperWidth;
        $this->paperHeight = $paperHeight;
        $this->population = array();
        $this->fitnesses = array();
        $this->bestFitness = PHP_INT_MAX;
        $this->generationCount = 0;

        for ($i = 0; $i < $populationSize; $i++) {
            $individual = new Rectangle(0, 0, rand($rectangleWidth - 10, $rectangleWidth + 10), rand($rectangleHeight - 10, $rectangleHeight + 10));
            $this->population[$i] = $individual;
            $this->fitnesses[$i] = $this->calculateFitness($individual);
        }
    }

    private function calculateFitness($individual)
    {
        $fitness = 0;
        for ($i = 0; $i < $this->rectangleCount; $i++) {
            $x = intval($individual->x + $this->rectangleWidth * $i);
            $y = intval($individual->y + $this->rectangleHeight * ($this->rectangleCount - $i - 1));
            if ($x + $this->rectangleWidth <= $this->paperWidth && $y + $this->rectangleHeight <= $this->paperHeight) {
                $fitness += abs($x - intval($this->paperWidth / 2)) + abs($y - intval($this->paperHeight / 2));
            } else {
                return PHP_INT_MAX; // 不可行解，返回一个极大值  
            }
        }
        return $fitness;
    }

    public function solve()
    {
        while ($this->generationCount < 100) {
            $this->generateOffspring();
            $this->generationCount++;
            if ($this->getBestFitness() < $this->bestFitness) {
                $this->bestFitness = $this->getBestFitness();
            }
        }
        return $this->getBestIndividual();
    }

    // generateOffspring方法未完继续  
    private function generateOffspring()
    {
        $offspring = array();
        for ($i = 0; $i < $this->populationSize; $i++) {
            $parent1Index = rand(0, $this->populationSize - 1);
            $parent2Index = rand(0, $this->populationSize - 1);
            $parent1 = $this->population[$parent1Index];
            $parent2 = $this->population[$parent2Index];
            $child = new Rectangle(intval(($parent1->x + $parent2->x) / 2), intval(($parent1->y + $parent2->y) / 2), abs($parent1->width - $parent2->width) / 2, abs($parent1->height - $parent2->height) / 2);
            if ($child->width >= $this->rectangleWidth && $child->height >= $this->rectangleHeight) {
                $offspring[] = $child;
            }
        }
        foreach ($offspring as $individual) {
            $this->population[] = $individual;
            $this->fitnesses[] = $this->calculateFitness($individual);
        }
    }

    public function getBestIndividual()
    {
        $bestIndividualIndex = 0;
        for ($i = 1; $i < $this->populationSize; $i++) {
            if ($this->fitnesses[$i] < $this->fitnesses[$bestIndividualIndex]) {
                $bestIndividualIndex = $i;
            }
        }
        return $this->population[$bestIndividualIndex];
    }

    public function getBestFitness()
    {
        return $this->fitnesses[0];
    }
    public function getIndividualCoordinates($individual) {  
        $x = $individual->x;  
        $y = $individual->y;  
        $width = $individual->width;  
        $height = $individual->height;  
        return array($x, $y, $width, $height);  
    }  
   
}

$solver = new GSASolver(50, 5, 240, 299, 889, 1194);  
$solver->solve();  
$bestIndividual = $solver->getBestIndividual();  
$coordinates = $solver->getIndividualCoordinates($bestIndividual);  
echo "Best Individual Coordinates: ";  
echo $coordinates[0] . ", " . $coordinates[1] . ", " . $coordinates[2] . ", " . $coordinates[3] . "\n";


p($bestIndividual);