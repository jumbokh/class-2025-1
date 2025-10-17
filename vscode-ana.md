url: https://zhuanlan.zhihu.com/p/11715106932
VSCode与Anaconda无缝连接
feng90
feng90
young astronomer
​关注他
收录于 · 天文笔记
6 人赞同了该文章
步骤 1：安装必备软件
1. 安装 Anaconda
下载并安装 Anaconda (或 Miniconda)。
安装完成后，确保将 Anaconda 添加到系统路径（勾选安装向导中的选项）。
2. 安装 VSCode
下载并安装 Visual Studio Code。
步骤 2：安装 VSCode 扩展
打开 VSCode。
在左侧的扩展图标中，搜索并安装以下扩展：
Python：由微软提供，用于支持 Python 开发。
Jupyter（可选）：如果需要运行 Jupyter Notebook。
步骤 3：配置 VSCode 使用 Anaconda 环境
打开 VSCode 并按下 Ctrl + Shift + P 调出命令面板。
输入并选择 "Python: Select Interpreter"。
在出现的列表中，选择对应的 Anaconda 环境。例如：csharp
(base) C:\Users\<YourUsername>\anaconda3\python.exe 
如果你的环境未显示：

确保 Anaconda 已正确安装。
打开终端并运行以下命令，查看现有环境：bash
conda info --envs 
手动添加解释器：
在命令面板中选择 "Python: Enter Interpreter Path"。
点击 "Find" 或手动输入解释器路径，例如：php
C:\Users\<YourUsername>\anaconda3\envs\<env_name>\python.exe 


步骤 4：设置终端默认使用 Anaconda
打开 VSCode 的设置文件：
按 Ctrl + ,，搜索 "Terminal › Integrated › Shell: Windows"。


将终端设置为使用 Anaconda Prompt：json
复制代码
{ "terminal.integrated.defaultProfile.windows": "Anaconda Prompt" } 
如果需要手动设置路径：

添加终端配置到 settings.json：
"terminal.integrated.profiles.windows": { "Anaconda Prompt": { "path": "C:\\Users\\<YourUsername>\\anaconda3\\Scripts\\conda.exe", "args": ["shell.cmd", "/K", "C:\\Users\\<YourUsername>\\anaconda3\\condabin\\activate"] } }, "terminal.integrated.defaultProfile.windows": "Anaconda Prompt" 
步骤 5：运行 Python 代码
创建一个 .py 文件。
选择环境后，在顶部点击 Run 按钮或按 F5 运行代码。
如果需要运行 Jupyter Notebook，请安装 Jupyter 扩展，并确保 .ipynb 文件关联正确。
步骤 6：在终端中激活环境
在 VSCode 的终端中，激活你的 Conda 环境：

conda activate <env_name>
例如：

conda activate my_env
通过以上配置，你的 VSCode 应该可以完美运行 Anaconda 环境中的 Python 代码。如果遇到任何问题，请分享具体报错信息以便进一步帮助。
