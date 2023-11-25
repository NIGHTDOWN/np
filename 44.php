<?php  
  
class Polygon {  
    protected $sides;  
    protected $points;  
  
    public function __construct($sides) {  
        $this->sides = $sides;  
        $this->points = [];  
        $this->generate();  
    }  
  
    protected function generate() {  
        $radius = rand(10, 50);  
        $angle = rand(0, 360);  
        for ($i = 0; $i < $this->sides; $i++) {  
            $x = $radius * cos(deg2rad($angle)) + rand(-50, 50);  
            $y = $radius * sin(deg2rad($angle)) + rand(-50, 50);  
            $this->points[] = [$x, $y];  
            $angle += rand(5, 30);  
        }  
    }  
  
    public function getSides() {  
        return $this->sides;  
    }  
  
    public function getPoints() {  
        return $this->points;  
    }  
}  
  
class Paper {  
    protected $width;  
    protected $height;  
    protected $polygons = [];  
  
    public function __construct($width, $height) {  
        $this->width = $width;  
        $this->height = $height;  
    }  
  
    public function addPolygon(Polygon $polygon) {  
        if ($this->isInside($polygon)) {  
            $this->polygons[] = $polygon;  
        }  
    }  
  
    protected function isInside(Polygon $polygon) {  
        $points = $polygon->getPoints();  
        $minX = min([$points[0][0], $points[1][0], $points[2][0]]);  
        $maxX = max([$points[0][0], $points[1][0], $points[2][0]]);  
        $minY = min([$points[0][1], $points[1][1], $points[2][1]]);  
        $maxY = max([$points[0][1], $points[1][1], $points[2][1]]);  
        return $minX >= 0 && $maxX <= $this->width && $minY >= 0 && $maxY <= $this->height;  
    }  
  
    public function getPolygons() {  
        return $this->polygons;  
    }  
}  
  
class GeneticAlgorithm {  
    protected $populationSize;  
    protected $eliteCount;  
    protected $mutationRate;  
    protected $generations;  
    protected $polygons = [];  
  
    public function __construct($populationSize, $eliteCount, $mutationRate, $generations) {  
        $this->populationSize = $populationSize;  
        $this->eliteCount = $eliteCount;  
        $this->mutationRate = $mutationRate;  
        $this->generations = $generations;  
    }  
  
    public function run() {  
        for ($i = 0; $i < $this->generations; $i++) {  
            $parents = $this->selectParents();  
            $child = $this->crossover($parents);  
            $child = $this->mutate($child);  
            $this->polygons[] = new Polygon($child['sides']);  
            $this->polygons[] = new Polygon($child['sides']);  
            $this->polygons[] = new Polygon($child['sides']);  
        }  
    }  
  
    protected function selectParents() {  
        $parents = [];  
        foreach ($this->polygons as $polygon) {  
            $sides = $polygon->getSides();  
            if ($sides == 3 || $sides == 4 || $sides == 5) {  
                $parents[] = $polygon;  
            }  
        }  
        return $parents;  
    }

    protected function crossover(array $parents) {  
        $childPoints = [];  
        $pointIndex1 = rand(0, count($parents[0]->getPoints()) - 1);  
        $pointIndex2 = rand(0, count($parents[1]->getPoints()) - 1);  
        for ($i = 0; $i < count($parents[0]->getPoints()); $i++) {  
            if (rand(0, 1) == 0) {  
                $childPoints[] = $parents[0]->getPoints()[$pointIndex1];  
                $pointIndex1 = ($pointIndex1 + 1) % count($parents[0]->getPoints());  
            } else {  
                $childPoints[] = $parents[1]->getPoints()[$pointIndex2];  
                $pointIndex2 = ($pointIndex2 + 1) % count($parents[1]->getPoints());  
            }  
        }  
        return ['sides' => $parents[0]->getSides(), 'points' => $childPoints];  
    }  
  
    protected function mutate(array $child) {  
        $mutatedChild = [];  
        $mutatedSide = rand(0, 2);  
        $points = $child['points'];  
        $sides = $child['sides'];  
        if (rand(0, 1) == 1) {  
            $mutatedSide = $sides == 3 ? 4 : $sides == 4 ? 3 : $sides == 5 ? 3 : $sides;  
        }  
        $mutatedChild['sides'] = $mutatedSide;  
        $mutatedChild['points'] = [];  
        for ($i = 0; $i < count($points); $i++) {  
            if (rand(0, 1) == 1) {  
                $x = $points[$i][0] + rand(-5, 5);  
                $y = $points[$i][1] + rand(-5, 5);  
                $mutatedChild['points'][] = [$x, $y];  
            } else {  
                $mutatedChild['points'][] = $points[$i];  
            }  
        }  
        return $mutatedChild;  
    }  
  
    public function getPolygons() {  
        return $this->polygons;  
    }  
}  
  
// Main code  
$paper = new Paper(100, 100);  
$geneticAlgorithm = new GeneticAlgorithm(100, 10, 0.1, 10);  
$geneticAlgorithm->run();  
$polygons = $geneticAlgorithm->getPolygons();  
$totalPolygons = count($polygons);  
// $totalSides = array_sum([$polygon->getSides() for $polygon in $polygons]);  
foreach ($polygons as $polygon) {  
    $points = $polygon->getPoints();  
    foreach ($points as $point) {  
        echo "(" . $point[0] . ", " . $point[1] . ") ";  
    }  
    echo "\n";  
}  
echo "Total polygons: " . $totalPolygons . "\n";  
echo "Total sides: " . $totalSides . "\n";