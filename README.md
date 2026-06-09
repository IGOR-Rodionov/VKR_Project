# Методы динамической, пространственной и частотной обработки звука

В данном репозитории находится исходный код программного модуля, написанного в рамках выполнения выпускной квалификационной работы. Программный модуль предназнчен для интеграции в аудио-редакторы и художественной обработки звука, осуществляемой при помощи различных методов динамической, пространственной и частотной обработки звука. Модуль реализован на языке программирования `C++` с использованием фреймворка `JUCE`. Логика разработанной системы была разделена по классам, каждый из которых выполняет свою функцию.

## Интерфейс программного модуля

За реализацию графического интерфейса программного модуля отвечает класс `PluginEditor`, `DialLookAtFeel` и файл `GUIprops`.

### Класс DialLookAtFell

### Класс PluginEditor

Класс `PluginEditor` является наследником базового класса `juce::AudioProcessorEditor`, который используется для создания пользовательских графических интерфейсов. Также он является наследником ещё 2 классов —  `juce::ToggleButton::Listener` и `juce::ComboBox::Listener`, которые используется для отслеживания изменения значений кнопок и выпадающих списков внутри данного класса. Класс содержит десятки полей, так как каждый элемент интерфейса объявлен в качестве поля. Основные элименты интерфейса — это ползунки `juce::Slider`, которые используется для регулировки значений параметров обработки. Другими элементами управления является радиокнопки `juce::ToggleButton`, использующиеся для включений различных опций обработки; выпадающее меню `juce::ComboBox`, использующиеся для выбра опции из нескольких вариантов, и одна кнопка `juce::TextButton`, применяемая для загрузки файла по нажатию. Чтобы понимать, за какой параметр отвечает конкретный элемент интерфейса, в классе объявляются поля подписи `juce::Label`, и группы `juce::GroupComponent`, чтобы их было удоюнее искать. Все поля, имеющий общий тип, объединяются в векторы для более удобного взаимодействия с множеством всех полей разом, что происходит достаточно часто, так как элменты интерфейса имеют общие черты.

В конструкторе класса `PluginEditor` следует настроить поля интерфейса и вывести их на интерфейсе, используя метод `addAndMakeVisible()`. Самой частой настройкой элементов является настройка их подписей. Для настройки подписи единиц измерений у ползунков используется метод `setTextValueSuffix()` и для настройки текста подписей и групп — метод `setText()`. Другими важными методами являются метод привязка подписей к ползункам через метод `attachToComponent()`, объединение радиокноп в одну группу с помощью метода `setRadioGroupId()`, чтобы не было возможны активировать сразу несколько кнопок из одной группы и привязка вызова функции при нажатии на одну из кнопок с помощью метода `onClick = []{}`. Но помимо настройки элементов интерфейса здесь происходит настройка и самого окна интерфейса. Сначала с помощью функции `setSize()` задаётся размер окна, затем вызовом метода `setResizable()` разрешается масштабирования окна по вертикали и горизонтали и наконец с помощью меоды `setResizeLimits()` устанавливается лимит масштабирования, равный 25 % от изначального размера окна.
```
setSize(1000, 675);
juce::AudioProcessorEditor::setResizable(true, true);
juce::AudioProcessorEditor::setResizeLimits(getWidth() * 0.75,
        getHeight() * 0.75, getWidth() * 1.75, getHeight() * 1.75);
juce::AudioProcessorEditor::getConstrainer()->setFixedAspectRatio(1.49);
```

Также важно не забыть о деструкторе классе `PluginEditor`, в котором через цикл удаляется пользовательский внешний вид ползунков. После завершения цикла вектор с полузнками очищается и освобождается не используемая память, выделенная для хранения элементов данного вектора.
```
for (auto& slider : sliders)
{
    slider->setLookAndFeel(nullptr);
}
sliders.clear();
sliders.shrink_to_fit();
```

Первым переопределённым базовым методом класса `juce::AudioProcessorEditor` является метод`paint()`, который используется для рисование в окне интерфейса. Данный метод использовался только для заливки заднего фона градиентом от фиолетового до чёрного цвета. Сначала с помощью метода `setGradientFill()` задаётся правило для градиентной заливки, а затем с помощью метода `fillRect()` задаётся область закрашивания. Так в данный метод был передан `getLocalBounds()`, то закрасится всё окно интерфейса.
```
g.setGradientFill(juce::ColourGradient::vertical(juce::Colour::fromRGB(40, 42, 53).darker(0.75f),
        getHeight(), juce::Colour::fromRGB(40, 24, 53).brighter(0.02f), getHeight() * 0.4));
g.fillRect(getLocalBounds());
```
Более важным методом является `resized()`, который определяет отображение элементов при изменении размера окна. Сначала в теле метода задаются размеры и отступы для всех элемнтов интерфейса. Затем, используя методы `setBounds()`, задаётся область, в которой должен быть отрисован данный элемент. Принцип отображения у всех элементов интерфейса одиноков, но здесь важно обратить внимание на 2 вещи. Во-первых, размеры и отспупы элементов расчитываются относительно текущего размера окна, так как они использует значения методов `getWidth()` и `getHeight()`. Это означает, что при изменении размера окна, размер элементов тоже изменится. Во-вторых, только у самого первого элемента область прорисовки задана с жёсткими рамками, остальные элементы высчитывают свою область относительно расположения предыдущего, то есть при изменении размеров окна элементы сохранят своё относительное расположение и нибудут накладываться друг на друга.

```
auto LeftMargin = getWidth() * 0.004;
auto UpMargin = getHeight() * 0.1;

auto SliderWidth = getWidth() * 0.14;
auto SliderMargin = 1.1;

auto WidthMargin = SliderMargin * 1.15;

auto CompressorMargin = 1.37;
auto secondRowHeight = 1.3;

auto ButtonSize = getWidth() * 0.035;
auto ButtonMargin = 1.25;

frequencySlider.setBounds(LeftMargin, UpMargin, SliderWidth, SliderWidth);
orderSlider.setBounds(frequencySlider.getX() + frequencySlider.getWidth() * SliderMargin, UpMargin, SliderWidth, SliderWidth);

filterTypeBox.setBounds(LeftMargin * 6, UpMargin + frequencySlider.getHeight() * 1.1, SliderWidth * 1.8, ButtonSize);

DelaySlider.setBounds(orderSlider.getX() + SliderWidth * 1.2, UpMargin, SliderWidth, SliderWidth);
BalanceSlider.setBounds(DelaySlider.getX() + DelaySlider.getWidth() * SliderMargin, UpMargin, SliderWidth, SliderWidth);
WidthSlider.setBounds(BalanceSlider.getX() + BalanceSlider.getWidth() * SliderMargin, UpMargin, SliderWidth, SliderWidth);

chorusButton.setSize(ButtonSize, ButtonSize);
feedbackButton.setSize(ButtonSize, ButtonSize);

feedbackButton.setBounds(DelaySlider.getX(), DelaySlider.getY() + DelaySlider.getHeight() * 1.1, SliderWidth, ButtonSize);
chorusButton.setBounds(BalanceSlider.getX(), BalanceSlider.getY() + BalanceSlider.getHeight() * 1.1, SliderWidth, ButtonSize);

lowFuncTypeBox.setBounds(WidthSlider.getX(), WidthSlider.getY() + WidthSlider.getHeight() * 1.1, SliderWidth, ButtonSize);

delayGroup.setBounds(orderSlider.getX() + SliderWidth * 1.1, UpMargin / 2.5, SliderWidth * 2.13, feedbackButton.getY() + feedbackButton.getHeight() * 0.55);
chorusGroup.setBounds(WidthSlider.getX() * 0.98, UpMargin / 2.5, SliderWidth * 1.15, chorusButton.getY() + chorusButton.getHeight() * 0.55);

inputSlider.setBounds(LeftMargin, delayGroup.getY() + delayGroup.getHeight() * CompressorMargin, SliderWidth, SliderWidth);
outputSlider.setBounds(LeftMargin, inputSlider.getY() + inputSlider.getHeight() * secondRowHeight, SliderWidth, SliderWidth);

ioGroup.setBounds(LeftMargin * 2.7, inputSlider.getY() * 0.86, SliderWidth * 0.9, outputSlider.getY() / 1.5);

threshSlider.setBounds(inputSlider.getX() + inputSlider.getWidth() * SliderMargin, inputSlider.getY(), SliderWidth, SliderWidth);
compMixSlider.setBounds(threshSlider.getX() + threshSlider.getWidth(), threshSlider.getY() + threshSlider.getHeight() * secondRowHeight / 2, SliderWidth, SliderWidth);
ratioSlider.setBounds(compMixSlider.getX() + compMixSlider.getWidth(), threshSlider.getY(), SliderWidth, SliderWidth);
attackSlider.setBounds(threshSlider.getX(), inputSlider.getY() + inputSlider.getHeight() * secondRowHeight, SliderWidth, SliderWidth);
releaseSlider.setBounds(ratioSlider.getX(), inputSlider.getY() + inputSlider.getHeight() * secondRowHeight, SliderWidth, SliderWidth);

compressorGroup.setBounds(threshSlider.getX(), ioGroup.getY(), SliderWidth * 3, ioGroup.getHeight());

limiterThreshSlider.setBounds(ratioSlider.getX() + ratioSlider.getWidth() * 1.15, threshSlider.getY(), SliderWidth, SliderWidth);
limiterReleaseSlider.setBounds(limiterThreshSlider.getX(), inputSlider.getY() + inputSlider.getHeight() * secondRowHeight, SliderWidth, SliderWidth);

limiterGroup.setBounds(limiterThreshSlider.getX() * 1., ioGroup.getY(), SliderWidth * 1.1, ioGroup.getHeight());

frequencyButton.setBounds(WidthSlider.getX() + WidthSlider.getWidth() * 1.25, UpMargin / 2.5, SliderWidth, ButtonSize);
delayEffectsButton.setBounds(frequencyButton.getX(), frequencyButton.getY() + ButtonSize * ButtonMargin, SliderWidth, ButtonSize);
reverbButton.setBounds(frequencyButton.getX(), delayEffectsButton.getY() + ButtonSize * ButtonMargin, SliderWidth, ButtonSize);
dynamicButton.setBounds(frequencyButton.getX(), reverbButton.getY() + ButtonSize * ButtonMargin, SliderWidth, ButtonSize);

loadBtn.setBounds(frequencyButton.getX() * 1.018, (dynamicButton.getY() + ButtonSize * ButtonMargin) * 1.15, SliderWidth, ButtonSize);
irName.setBounds(loadBtn.getX() * 0.99, loadBtn.getY() + loadBtn.getHeight() * 1.25, SliderWidth, ButtonSize);

frequencyGroup.setBounds(LeftMargin, UpMargin / 6, SliderWidth * 2, filterTypeBox.getY() + ButtonSize * 1.22);
delayEffectsGroup.setBounds(orderSlider.getX() + SliderWidth, UpMargin / 6, SliderWidth * 3.5, feedbackButton.getY() + feedbackButton.getHeight() * 1.22);
dynamicProcGroup.setBounds(LeftMargin, inputSlider.getY() * 0.8, SliderWidth * 5.6, ioGroup.getHeight() * 1.1);
processingSettingsGroup.setBounds(WidthSlider.getX() + WidthSlider.getWidth() * 1.2, UpMargin / 6, SliderWidth * 1.3, ButtonSize * ButtonMargin * 4.2);
reverbGroup.setBounds(processingSettingsGroup.getX(), dynamicButton.getY() + ButtonSize * ButtonMargin, SliderWidth * 1.3, ButtonSize * 5);
```

### Файл GUIprops

Уже упоминалось, что однотипные элемнты интерфейса имеют общие настройки. Поэтому включение этих настроек было реализованно в отдельных функциях, которые находяться внутри файла `GUIprops.cpp`. В нём сущесвтует всего 7 методов.

Первый метод `buttonClicked()`, который унаследован от базового класса `juce::ToggleButton::Listener`. Так как в конструкторе класса `PluginEditor` радиокнопки уже привезали вызов функции по нажатию, то остаётся только реализовать саму логику. Сначала нужно определить, какая кнопка была нажата, что делается путём сравнения аргумента функции `button` с конкретной радиокнопкой. Если их значения совпадают, то нажатая кнопка определена. Теперь внутри блока `if(){}` остаётся только включить соответсвующую опцию обработки, используя объект `audioProcessor`, в котором ихранится вся обработка данного модуля.

Следующий метод `comboBoxChanged()`, который аналогичным образом обрабатывается изменение значений элементов интерфейса, но уже не радиокнопок, а выпадающего списка.

Другой метод в файле — это метод `setCommonSliderProps()`, который включает настройки для переданного ползунка. Так как все ползунки находятся в векторе, то в конструкторе класса в цикле для каждого ползунка вызывалась данная функция. То есть настройки, которые включаются в данной функции, будут присутсвовать в каждом ползунке интерфейса. Сначала с помощью метода `addAndMakeVisible()` ползунок становится видимым и добавляется в интерфейс, а затем настраивается внешний ползунка. С помощью `setSliderStyle()` настраивается вид ползунка, в данном случае он становится круглым и вращающимся, с помощью `setTextBoxStyle()` настраивается отображение текста со значением, а именно оно будет отображатся снизу от ползунка. Метод `setLookAndFeel()` позволяет включить пользовательский внешний вид ползунков, который был настроен в классе `DialLookAtFell`. Далее идёт настройка цвета ползунка, и наконец настройка тен и ползунка, чтобы он был более чётким на фоне интерфейса.

```
addAndMakeVisible(slider);

slider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 140, 36);
slider.setLookAndFeel(&customDialAF);

slider.setColour(juce::Slider::ColourIds::textBoxOutlineColourId, juce::Colours::transparentBlack);
slider.setColour(juce::Slider::ColourIds::backgroundColourId, juce::Colours::whitesmoke.darker(0.8f));
slider.setColour(juce::Slider::ColourIds::rotarySliderOutlineColourId, juce::Colours::black.brighter(0.25));
slider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colour::fromRGB(251, 242, 252).darker(0.15));
slider.setColour(juce::Slider::ColourIds::thumbColourId, juce::Colour::fromRGBA(0.392f, 0.584f, 0.929f, 1.f).darker(1.f));

shadowProperties.radius = 24;
shadowProperties.offset = juce::Point<int>(0, 0);
shadowProperties.colour = juce::Colours::black;
SliderShadow.setShadowProperties(shadowProperties);
slider.setComponentEffect(&SliderShadow);
```
Метод `setCommonLabelProps()` и `setCommonButtonProps()` выполняют аналогичную функцию, но являются более компактными. `setCommonLabelProps()` настраивает общие элементы для подписей, а именно включает их отображение на интерфейсе, устанавливает шрифт и ег оразмеры, а также включает выравнивание подписи, в данном случае выравнивание по центру.

```
addAndMakeVisible(label);
label.setFont(juce::Font("Helvetica", 16.f, juce::Font::FontStyleFlags::bold));
label.setJustificationType(juce::Justification::centred);
```

## Логика программного модуля

### Класс LVCompressor

Класс `LVCompressor` является реализации компрессора. В классе содержится 8 методов, 5 из которых — это сет методы, и из 11 полей, 5 из которых это параметры.

Каждое из полей класса имеет тип float кроме одного — поле `_mix` является экземпляром класса `juce::SmoothedValue`. Объекты данного класса представляют из себе переменные одного из базовых типов, которые при изменение своего значения не просто присваевают себе новое, а плавно его изменяют. Данная особенность позволяет избежать некотрых артефактов, возникающих во время обработки сигнала в реальном времени при резком изменении значения переменной. Так же в улассе есть 4 поля, отвечающих за параметры компрессии: `_thresh`, отвечающий за порог, `_ratio` — коэффициент сжатия, `_attack`  — время атаки и `_release`  —  время спада. Остальные поля класса являются служебными и будут описаны при разборе методов.

Первым методом, который будет вызван в классе компрессора, является методов `prepare()`. Он используется для подготовки компрессора к обработке и инициализации некоторых класса полей. В данном случае в этотм методе установливается значение частоты дискретизации с помощью поля `_samplerate`, а также сбрасывается значение поля `_mix`.

Главным методом класса является `process()`, который и отвечает за обработку сигнала. Он получает входной сигнал с помощью аргумента `buffer`, считывает его данные методом `getArrayOfWritePointers()` и приступает к обработке. Сначала запускается цикл по числу выбор из сигнала, затем идёт ещё один цикл, пробигающий по каналам аудио файла. Внутри вложенного цикла вызывается методо `processSample()`, который перезаписывает значение текущей выборки сигнала. Как только цикл пройдёт все выборки исходного сигнала, то метод завершит свою работу.

```
void LVCompressor::process(juce::AudioBuffer<float>& buffer) noexcept
{
    auto data = buffer.getArrayOfWritePointers();

    for (int sample = 0; sample < buffer.getNumSamples(); ++sample)    
    {
        for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
        {
            data[ch][sample] = processSample(data[ch][sample]);
        }
    }
}
```
Метод `processSample()` подсчитывает новое значение одной конкретной выборки сигнала. Сначала метод переводит значения параметров `_attack` и `_release` из милисекунд в выборки и записывает полученные значения в локальны переменные.

```
auto alphaAttack = std::exp((std::log(9) * -1.) / (_samplerate * _attack));
auto alphaRelease = std::exp((std::log(9) * -1.) / (_samplerate * _release));
```

Затем значение выборки сигнала записывается в локальную переменную, чтобы обезопасить себя от непреднамеренного изменения зна чения исходного сигнала. После считается значение данной выборки по модулю, и оно переводится в цифровые децибелы с помощью метода `juce::Decibels::gainToDecibels(x_Uni)`
```
const auto x = input;
auto x_Uni = abs(x);
auto x_dB = juce::Decibels::gainToDecibels(x_Uni);
```
Последним шагом перед компрессией является проверка числа на минус бесконечность. Чем меньше по модулю исходная выборка сигнала, тем ближе она будет к минус бесконечности при переводе в цифровые децибелы. Чтобы избежать возможные математические ошибки, устанавляивается минмальнон значение сигнала, равное -96 дБ.
```
if (x_dB < -96.)
{
    x_dB = -96.;
}
```
Теперь метод приступает к компрессии сигнала в соответсвии с формулами (1.12)‐(1.14). Поле `gainSC` используется для хранения значения G после формулы (1.12), затем разница между `gainSC` и исходным сигналом записывается в поле `gainChange_dB`. Это значение сравнивается с предыдушем значением переменной, которое хранится в поле `gainSmoothPrevious`, и применяется формула (1.13). И наконец новое значение записывается `gainSmoothPrevious` и считается значение обработанной выборки по формуле (1.14).

```
if (x_dB > _thresh)
{
    gainSC = _thresh + (x_dB - _thresh) / _ratio;
}
else
{
    gainSC = x_dB;
}

gainChange_dB = gainSC - x_dB;
if (gainChange_dB < gainSmoothPrevious)
{
    gainSmooth = ((1 - alphaAttack) * gainChange_dB) + (alphaAttack * gainSmoothPrevious);
}
else
{
    gainSmooth = ((1 - alphaRelease) * gainChange_dB) + (alphaRelease * gainSmoothPrevious);
}

gainSmoothPrevious = gainSmooth;
auto wetInput = x * juce::Decibels::decibelsToGain(gainSmooth);
```
Посчитанное значение называют мокрым (wet), а исходное необработанное значение — сухим (dry). В зависимости от влечины `_mix` можно варировать содержание обработанного и исходжного звука в результирующем сигнале: если `_mix` равен 1, то сигнал будет содержать только обработанный звук, а если 0 — только исходный.
```
auto mix = (1. - _mix.getNextValue()) * x + wetInput * _mix.getNextValue();
return mix;
```

### Класс NOrderButterworth

### Класс CustomConvolution

### Класс PluginProcessor
