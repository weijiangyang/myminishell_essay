#ifndef SIGNAL_H
#define SIGNAL_H

void sigint_prompt(int sig);
void setup_prompt_signals(void);
void setup_child_signals(void);
void setup_parent_exec_signals(void);

#endif