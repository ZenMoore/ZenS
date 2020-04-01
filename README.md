## ZenS host programme using Due as device.
This is for the project 'ZenS' of our organization 'sino-crdc'.

#### Structure
- main: 入口处，检测 Due 按下的按钮，根据按钮控制分流。
- none_to_demo: 没按下按钮时候，向 Unity3D 持续传送角度等数据指令
- A_to_demo: 按下按钮 A 时候，向 Unity3D 持续传送角度等数据指令
- B_to_gestRecog_demo: 按下按钮 B 的时候，向 gestRecog 传送\[6, 256\]序列数据，并向 Unity3D 传送相应指令