# pinyin-input-method
> 简陋的拼音输入法，集成开发环境为vs2017
- 命令行界面下输入和选择
- 通过读入pinyin.txt和word.txt两个资源文件加载音节表和词库
- 全拼输入（必要时需加'作为分隔符）
- 可选择记录和读入用户细胞词库
***
### example

  > ![loading...](https://github.com/ilan-NJU/crude-pinyin/blob/master/img/%E4%BD%BF%E7%94%A8%E7%A4%BA%E4%BE%8B.png)

---
### to be improved

- 多音字有待实现
- 没有更高级的词法分析，如期期艾艾 拼成qiqiaiai第二个音节会默认向后匹配为qia，从而无法识别，需写成qiqi'aiai才可正确识别
- 资源文件可按汉字常用程度来排序，进而按一定优先级给出候选
