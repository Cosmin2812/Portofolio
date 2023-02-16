#!/usr/bin/python3 -u
# Copyright 2020 Darius Neatu (neatudarius@gmail.com)

from glob import glob
from sys import platform
import os
import pathlib
import subprocess
import time
import recordclass

from .parser import get_config
from .utils import *

f1=None

def update_grade_vmr(config, points, reason):
    pass


def update_grade(config, points):
    assert config is not None

    config.grade += points
    if points < 0:
        log('{} points penalty applied!\n'.format(points))


def check_deps(config):
    assert config is not None

    log(' -------------------- ')
    log('| CHECK DEPENDENCIES |')
    log(' -------------------- ')

    deps = config.deps

    child = subprocess.run(['uname', '-a', ], shell=False,
                           stdin=subprocess.DEVNULL,
                           stdout=subprocess.PIPE,
                           stderr=subprocess.DEVNULL,
                           )
    assert child.returncode == 0, child.stderr
    log('system: {}'.format(platform))
    log('config: {}'.format(extract_stdout(child).split('\n')[0]))

    for dep in deps:
        if dep == 'valgrind' and platform == 'darwin': # valgrind is not supported on MAC OS X
            continue

        child = subprocess.run(['which', dep, ], shell=False,
                               stdin=subprocess.DEVNULL,
                               stdout=subprocess.DEVNULL,
                               stderr=subprocess.DEVNULL,
                               )
        if child.returncode != 0:
            log('{}: not installed! Cannot continue'.format(dep))
            return child.returncode

        child = subprocess.run([dep, '--version', ], shell=False,
                               stdin=subprocess.DEVNULL,
                               stdout=subprocess.PIPE,
                               stderr=subprocess.PIPE,
                               )
        if child.returncode != 0:
            log(extract_stderr(child))
            return child.returncode

        log('{}: installed, version: {}'.format(dep, extract_stdout(child).split('\n')[0]))

    log("")
    return 0


def make_build(config):
    assert config is not None

    log(' -------')
    log('| BUILD |')
    log(' -------')

    makefile = 'Makefile'
    if not pathlib.Path(makefile).is_file():
        reason = '{} is missing!'.format(makefile)
        log(reason)
        update_grade_vmr(config, -100, reason)
        return -1

    try:
        child = subprocess.run(['make', 'build', ], shell=False,
                               stdin=subprocess.DEVNULL,
                               stdout=subprocess.PIPE,
                               stderr=subprocess.PIPE,
                               )
    except Exception as e:
        log('UPS: {}'.format(str(e)))
        return -1

    if child.returncode != 0:
        reason = 'Compile ERRORS found. STOP. Please fix them.'
        log(reason)
        log('{}'.format(extract_stderr(child)))

        update_grade_vmr(config, -100, reason)
        return child.returncode

    if config.penalty_warnings > 0:
        warnings = child.stderr.decode("utf-8")
        jeg1 = 'make: warning:  Clock skew detected.  Your build may be incomplete.'
        jeg2 = 'make: Warning: File \'Makefile\' has modification time'
        warnings = list(filter(lambda x: not x.startswith(jeg1) and not x.startswith(jeg2) , warnings))
        if len(warnings) > 0:
            reason = 'WARNINGS found. Please fix them.'
            log(reason)
            log('{}'.format(''.join(warnings)))

            update_grade(config, -config.penalty_warnings)
            update_grade_vmr(config, -config.penalty_warnings, reason)

    for task in config.tasks:
        if not pathlib.Path(task.id).is_file() or not os.access(os.path.join('.', task.id), os.X_OK):
            log('[WARNING] Executable \'{}\' not found!'.format(task.id))

    log('Sources built successfully\n')
    return 0


def make_clean(config):
    assert config is not None

    log(' ------- ')
    log('| CLEAN |')
    log(' ------- ')

    try:
        child = subprocess.run(['make', 'clean', ], shell=False,
                               stdin=subprocess.DEVNULL,
                               stdout=subprocess.PIPE,
                               stderr=subprocess.PIPE,
                               )
    except Exception as e:
        log('UPS: {}'.format(str(e)))
        return -1

    if child.returncode != 0:
        reason = '\'make clean\' failed. Fix it!'
        log(reason)
        update_grade(config, -config.grade)
        update_grade_vmr(config, -config.grade, reason)

        log('{}'.format(extract_stdout(child)))
        return child.returncode
    else:
        log('\'make clean\' ran successfully\n')

    return 0


def check_readme(config):
    if config.penalty_readme <= 0:
        return 0  # check not enable

    log(' -------------- ')
    log('| CHECK README |')
    log(' -------------- ')

    log('Maximum possible penalty: {:g}p'.format(config.penalty_readme))

    readme = pathlib.Path('README')
    readme_points = 0
    if not readme.is_file():
        reason = '\'{}\' is missing!'.format(readme.name)
        log(reason)
        readme_points -= config.penalty_readme
    elif readme.stat().st_size == 0:
        reason = 'Empty \'{}\'!'.format(readme.name)
        log(reason)
        readme_points -= config.penalty_readme
    elif readme.stat().st_size < 64:
        reason = '\'{}\' too short! Nice try! :p'.format(readme.name)
        log(reason)
        readme_points -= config.penalty_readme
    else:
        log('Found \'{}\' ... Final points are given after manual grading!'.format(
            readme.name))
        readme_points = 0

    update_grade(config, readme_points)
    log('Your penalty: {:g}\n'.format(readme_points))

    return 0


def check_style(config, language):
    if config.coding_style <= 0:
        return 0  # check not enable

    log(' -------------------- ')
    log('| CHECK CODING STYLE |')
    log(' -------------------- ')

    log('Maximum possible penalty: {:g}p'.format(config.coding_style))

    cmd = '{} . 2>&1'.format(language.cs_checker)

    try:
        child = subprocess.run(cmd, shell=True,
                               stdin=subprocess.DEVNULL,
                               stdout=subprocess.PIPE,
                               stderr=subprocess.DEVNULL,
                               )
    except Exception as e:
        log('UPS: {}'.format(str(e)))
        return -1

    if language.is_c:
        if child.returncode != 0 or len(extract_stdout(child)) != 0:
            coding_style_grade = -(1.0 * max(config.grade, 0) /
                                config.tests_points) * config.coding_style

            reason = 'Coding style errors automatically detected.'
            update_grade_vmr(config, -config.coding_style, reason)
        else:
            coding_style_grade = 0
            reason = 'No coding style errors automatically detected. Final points are given after manual grading.'
            update_grade_vmr(config, coding_style_grade, reason)

        config.grade += coding_style_grade

        log('{}'.format(extract_stdout(child)))
        coding_style_grade = '{:g}'.format(coding_style_grade)
    elif language.is_py:
        log('Check skipped for py, but you can run "pylint *.py" / "autopep8 -i *.py" to improve your coding style!')
        coding_style_grade = 'N/A'

    # log('Note: The coding style points are uniformly distributed per task.')
    log('Your penalty: {}\n'.format(coding_style_grade))

    return 0


def run_test(task, test, language, use_valgrind=False, ):
    stage_name = 'test' if not use_valgrind else 'valgrind'
    indent_log()
    log('Running {}'.format(stage_name))

    cmd = None
    if not use_valgrind or platform == 'darwin': # valgrind not supported on MAC OS X
        # man timeout: 124 if command times out
        expected_error_code = 124

        cmd = '{}'.format(language.exec)
    else:
        # choose an error to be returned by valgrind
        expected_error_code = 69

        cmd = 'valgrind '              \
            '--leak-check=full '       \
            '--show-reachable=no '     \
            '--show-leak-kinds=all '   \
            '--error-exitcode={} '     \
            '--log-file={} '           \
            '{}'.format(expected_error_code, test.log_valgrind, language.exec)

    log('Running command: {}'.format(cmd))
    if task.memory:
        set_mem_bytes(task.memory)

    try:
        child = subprocess.run(cmd, shell=False if not use_valgrind else True,
                               stdin=open(
                                   test.input, 'r') if task.use_stdin else subprocess.DEVNULL,
                               stdout=open(
                                   test.output, 'w') if task.use_stdout else subprocess.DEVNULL,
                               stderr=subprocess.PIPE,
                               timeout=task.timeout if not use_valgrind else 500,
                               preexec_fn=limit_process_memory if task.memory and not use_valgrind else None,
                               )
    except subprocess.TimeoutExpired as e:
        log_replace('Running {:10s}: TLE: {}'.format(stage_name, str(e)))
        return -1
    except Exception as e:
        log_replace('Running {:10s}: UPS: {}'.format(stage_name, str(e)))
        return -1

    if child.returncode != 0:
        if not use_valgrind:
            log_replace(
                'Running {:10s}: UPS: exit code = {}'.format(stage_name, child.returncode))
        elif child.returncode == expected_error_code:
            log_replace('Running {:10s}: MEM_UPS'.format(stage_name))

        return child.returncode

    log_replace('\n')
    log_replace('Running {}: OK'.format(stage_name))
    unindent_log()
    return 0

def run_test_with_env_deps(task, test, language, use_valgrind=False, ):
    if task.use_env_deps:
        backup_cwd = os.getcwd()

        in_deps = '{}/in'.format(test.deps)
        out_deps = '{}/out'.format(test.deps)
        

        os.system('rm -rf {}/* 2>&1 1>/dev/null'.format(out_deps))
        os.system('mkdir -p {} 2>&1 1>/dev/null'.format(out_deps))
        os.system('cp {}/* {}'.format(in_deps, out_deps))

        os.chdir(out_deps)
        # log('cwd = {}'.format(os.getcwd()))


    ret = run_test(task, test, language, use_valgrind,)

    if task.use_env_deps:
        os.chdir(backup_cwd)

    return ret

def check_test(task, test):
    global f1
    stage_name = 'check'
    # indent_log()
    log('Running {:10s}'.format(stage_name))
    cmd = '{} {} {} {} {} {}'.format(
        task.grader, test.input, test.output, test.ref, test.points, test.id)

    # print(cmd)
    grade_file = '.check.grade'
    if pathlib.Path(grade_file).is_file():
        pathlib.Path(grade_file).unlink()

    try:
        child = subprocess.run(cmd, shell=True,
                               stdin=subprocess.DEVNULL,
                               stdout=subprocess.PIPE,
                               stderr=subprocess.PIPE,
                               )
    except Exception as e:
        log_replace('Running {:10s}: UPS: {}'.format(stage_name, str(e)))
        return -1


    if child.returncode != 0:
        if f1 != 'N/A':
            f1 = f1 / 2
        log_replace(
            'Running {}: WA: {}'.format(stage_name, extract_stderr(child)))
        return child.returncode

    assert pathlib.Path(grade_file).is_file(), grade_file
    test.grade = 0
    with open(grade_file, 'r') as f:
        test.grade = float(f.readlines()[0].rstrip())
    pathlib.Path(grade_file).unlink()

    warnings = extract_stderr(child)
    if f1 == 'N/A':
        # task 1
        f1 = extract_stdout(child).strip()
        if f1 == None or f1 == '':
            f1 = 0
        f1 = float(f1)
    else:
        # task 2
        f1_2 = extract_stdout(child).strip()
        if f1_2 == None or f1_2 == '':
            f1_2 = 0
        f1 = (f1 + float(f1_2)) / 2
    log_replace('running {}: OK: {}'.format(stage_name, warnings))
    # unindent_log()
    return 0


def run_task(config, language, task):
    # log('{} ({:g}p) ...\n'.format(task.id, task.points))

    for test in task.tests:
        log('Task{}:'.format(int(test.id[1]) + 1))

        stages = [
            lambda: run_test_with_env_deps(task, test, language),
            lambda: check_test(task, test),
        ]

        if language.valgrind and (task.use_valgrind or test.use_valgrind):
            stages.append(lambda: run_test_with_env_deps(task, test, language, use_valgrind=True))

        code = 0
        for stage in stages:
            code, _ = run_and_measure(stage)
            if code != 0:
                break

        if code != 0:
            test.grade = 0

        task.grade += test.grade
        indent_log()
        log('')
        log('Task{}: {:g}/{:g}\n'.format(int(test.id[1]) + 1, test.grade, test.points))
        unindent_log()

    log('All tasks: {:g}/{:g}\n'.format(task.grade, task.points))

    if task.grade < task.points:
        update_grade_vmr(config, task.grade - task.points, 'Failed tests for task {}'.format(task.id))

    return 0


def run_tasks(config, language):
    log(' ------------------- ')
    log('| RUNNING ALL TASKS |')
    log(' ------------------- ')

    # if not 'ONLINE_JUDGE' in os.environ:
    #     os.system('./check_utils/.surprise/play_testing.sh &')
    #     time.sleep(1)
    #     log('\n\n\n')
    #     sys.stdout.flush()

    for task in config.tasks:
        run_task(config, language, task)
        config.grade += task.grade

    if not 'ONLINE_JUDGE' in os.environ:
        os.system('killall mpg123 >/dev/null 2>&1')

    return 0


def print_grade_vmr(config):
    if 'ONLINE_JUDGE' in os.environ and config.grade_vmr:
        log('generating grade.vmr')

        with open('grade.vmr', 'w') as grade_vmr:
            for line in config.grade_vmr:
                log('{}'.format(line))
                grade_vmr.write('{}\n'.format(line))


def checker_run_language(language):
    global f1
    log(' ==========================')
    log('| Running challenge for {} |'.format(language.name))
    log(' ==========================\n')

    os.system("rm -f urls-predictions.out")
    os.system("rm -f traffic-predictions.out")

    config = get_config()
    assert config is not None
    config.grade = 0
    f1 = 'N/A'

    checks = [
        lambda: check_deps(config),
        lambda: make_build(config) if language.name == 'c' else 0,
        lambda: run_tasks(config, language) or config.grade == 0,
        lambda: check_style(config, language),
        lambda: check_readme(config),
        lambda: make_clean(config) if language.name == 'c' else 0,
        # lambda: print_grade_vmr(config),
    ]

    for check in checks:
        ret = check()
        if ret != 0:
            break

    # don't give negative grades
    config.grade = config.grade if config.grade > 0 else 0
    log(' ------- ')
    log('| FINAL |')
    log(' ------- ')
    log('Grade ({}): {}/100 ({}/{})'.format(
        language.name,
        config.grade, config.grade / 100 * language.points, language.points)
    )

    f1 = '{}'.format(f1)
    if 'ONLINE_JUDGE' in os.environ:
        try:
            from .crypto.encrypt import encrypt
            f1 = encrypt(f1)
        except ImportError as e:
            print(e)
            f1 = 'N/A (ping Darius)'
    log('F1 ({}): {}\n'.format(language.name, f1))

    # if not 'ONLINE_JUDGE' in os.environ:
    #     if config.grade < 100:
    #         os.system('./check_utils/.surprise/play_failed.sh')
    #     elif config.grade >= 100 and f1 != 'N/A':
    #         if float(f1) <= 0.76:
    #             os.system('./check_utils/.surprise/play_congrats_gta.sh')
    #         else:
    #             os.system('./check_utils/.surprise/play_congrats_champions.sh')

def checker_run(language=None):
    fields = [
        'name',
        'points',
        'is_c',
        'is_py',
        'exec',
        'valgrind',
        'cs_checker',
    ]
    c_config = {
        'name': 'c',
        'points': 0.5,
        'is_c': True,
        'is_py': False,
        'exec': './my_av',
        'valgrind': True,
        'cs_checker': './check_utils/cs/c/cs.sh .'
    }
    py_config = {
        'name': 'py',
        'points': 0.2,
        'is_c': False,
        'is_py': True,
        'exec': './my_av.py',
        'valgrind': False,
        'cs_checker': './check_utils/cs/py/cs.sh .'
    }

    languages = [
        recordclass.recordclass('Config', fields)(**c_config),
        recordclass.recordclass('Config', fields)(**py_config),
    ]

    if language:
        languages = list(filter(lambda lang: lang.name == language, languages))

    for language in languages:
        checker_run_language(language)
